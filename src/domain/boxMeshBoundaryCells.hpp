#ifndef ABLATELIBRARY_BOXMESHBOUNDARYCELLS_HPP
#define ABLATELIBRARY_BOXMESHBOUNDARYCELLS_HPP

#include <memory>
#include <parameters/parameters.hpp>
#include <vector>
#include "domain.hpp"

namespace ablate::domain {
class BoxMeshBoundaryCells : public Domain {
   private:
    static DM CreateBoxDM(const std::string& name, std::vector<int> faces, std::vector<double> lower, std::vector<double> upper, bool simplex = true);

    /**
     *
     * @param lower
     * @param upper
     * @param scaleFactor the petsc input may specify scale factor.  This allows us to scale lower/upper to make sure we find the correct bounds
     * @param preModifiers
     * @param postModifiers
     * @return
     */
    static std::vector<std::shared_ptr<modifiers::Modifier>> AddBoundaryModifiers(std::vector<double> lower, std::vector<double> upper, double scaleFactor,
                                                                                  std::vector<std::shared_ptr<modifiers::Modifier>> preModifiers,
                                                                                  std::vector<std::shared_ptr<modifiers::Modifier>> postModifiers);

    inline const static std::string interiorCellsLabel = "interiorCells";
    inline const static std::string entireDomainLabel = "domain";
    inline const static std::string boundaryFacesLabel = "boundaryFaces";
    inline const static std::string boundaryCellsLabel = "boundaryCells";
    inline const static std::string boundaryCellsLeft = "boundaryCellsLeft";
    inline const static std::string boundaryCellsRight = "boundaryCellsRight";
    inline const static std::string boundaryCellsBottom = "boundaryCellsBottom";
    inline const static std::string boundaryCellsTop = "boundaryCellsTop";
    inline const static std::string boundaryCellsFront = "boundaryCellsFront";
    inline const static std::string boundaryCellsBack = "boundaryCellsBack";

   public:
    BoxMeshBoundaryCells(const std::string& name, const std::vector<std::shared_ptr<FieldDescriptor>>& fieldDescriptors, std::vector<std::shared_ptr<modifiers::Modifier>> preModifiers,
                         std::vector<std::shared_ptr<modifiers::Modifier>> postModifiers, std::vector<int> faces, const std::vector<double>& lower, const std::vector<double>& upper,
                         bool simplex = true, const std::shared_ptr<parameters::Parameters>& options = {});

    ~BoxMeshBoundaryCells() override;
};
}  // namespace ablate::domain

#endif  // ABLATELIBRARY_BOXMESHBOUNDARYCELLS_HPP
