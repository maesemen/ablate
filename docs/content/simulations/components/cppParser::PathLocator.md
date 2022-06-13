---
layout: default
title: cppParser::PathLocator
parent: Component List
grand_parent: Running Simulations
nav_exclude: true
---
# cppParser::PathLocator
## ablate::environment::Download
Downloads and relocates file at given url

## ablate::environment::GitHub
Downloads file/directory from github repo

repository (req) 
: (string) repository to download file from (must include owner/name)

path (req) 
: (string) the relative path in the repo

token
: (string) optional github [personal access token](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token) to access repo or bypass rate limits.

## cppParser::LocalPath*
Default path locator that returns specified local path


