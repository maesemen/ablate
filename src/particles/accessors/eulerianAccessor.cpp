#include "eulerianAccessor.hpp"

#include <utility>
#include "particles/particleSolver.hpp"

ablate::particles::accessors::EulerianAccessor::EulerianAccessor(bool cachePointData, std::shared_ptr<ablate::domain::SubDomain> subDomain, SwarmAccessor& swarm, PetscReal currentTime)
    : Accessor(cachePointData), subDomain(std::move(subDomain)), currentTime(currentTime), np(swarm.GetNumberParticles()) {
    // Resize and copy over the coordinates
    auto coodinatesField = swarm.GetData(ablate::particles::ParticleSolver::ParticleCoordinates);

    // Size up and copy the coordinates
    coordinates.resize(np * coodinatesField.numberComponents);
    coodinatesField.CopyAll(coordinates.data(), np);
}

ablate::particles::accessors::ConstPointData ablate::particles::accessors::EulerianAccessor::CreateData(const std::string& fieldName) {
    // Store the eulerianFieldInformation
    Vec locEulerianField;
    IS eulerianFieldIs;
    DM eulerianFieldDm;

    /* Get local field */
    const auto& eulerianField = subDomain->GetField(fieldName);
    subDomain->GetFieldLocalVector(eulerianField, currentTime, &eulerianFieldIs, &locEulerianField, &eulerianFieldDm) >> utilities::PetscUtilities::checkError;

    // Set up the interpolation
    DMInterpolationInfo interpolant;
    DMInterpolationCreate(PETSC_COMM_SELF, &interpolant) >> utilities::PetscUtilities::checkError;
    DMInterpolationSetDim(interpolant, subDomain->GetDimensions()) >> utilities::PetscUtilities::checkError;
    DMInterpolationSetDof(interpolant, eulerianField.numberComponents) >> utilities::PetscUtilities::checkError;

    // Copy over the np of particles
    DMInterpolationAddPoints(interpolant, np, coordinates.data()) >> utilities::PetscUtilities::checkError;

    /* Particles that lie outside the domain should be dropped,
    whereas particles that move to another partition should trigger a migration */
    DMInterpolationSetUp(interpolant, eulerianFieldDm, PETSC_FALSE, PETSC_TRUE) >> utilities::PetscUtilities::checkError;

    // Create a vec to hold the information
    Vec eulerianFieldAtParticles;
    VecCreateSeq(PETSC_COMM_SELF, np * eulerianField.numberComponents, &eulerianFieldAtParticles) >> utilities::PetscUtilities::checkError;

    // interpolate
    DMInterpolationEvaluate(interpolant, eulerianFieldDm, locEulerianField, eulerianFieldAtParticles) >> utilities::PetscUtilities::checkError;

    // Now cleanup
    DMInterpolationDestroy(&interpolant) >> utilities::PetscUtilities::checkError;
    subDomain->RestoreFieldLocalVector(eulerianField, &eulerianFieldIs, &locEulerianField, &eulerianFieldDm) >> utilities::PetscUtilities::checkError;

    // Get the raw array from the vec
    const PetscScalar* valueArray = nullptr;
    VecGetArrayRead(eulerianFieldAtParticles, &valueArray) >> utilities::PetscUtilities::checkError;

    // Clean up the vec when done being used
    RegisterCleanupFunction([eulerianFieldAtParticles, valueArray]() {
        Vec eulerianFieldAtParticlesTmp = eulerianFieldAtParticles;
        const PetscScalar* valueArrayTmp = valueArray;
        VecRestoreArrayRead(eulerianFieldAtParticlesTmp, &valueArrayTmp) >> utilities::PetscUtilities::checkError;
        VecDestroy(&eulerianFieldAtParticlesTmp) >> utilities::PetscUtilities::checkError;
    });

    // return the vec
    return {valueArray, eulerianField.numberComponents};
}
