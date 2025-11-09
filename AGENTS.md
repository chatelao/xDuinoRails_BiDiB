# Agent Guidelines

This file contains instructions for AI agents working on this repository.

## Development Process

Before starting any implementation work related to the BiDiB Arduino library, you **must** consult the `implementation_plan.md` document. This document outlines the agreed-upon development phases and the logical order of implementation.

After completing a development phase or a significant feature, you **must** update the `implementation_plan.md` to reflect the current status of the project.

## Project Structure and CI/CD

- **Arduino Library:** This project is planned as an Arduino library. The code should be structured accordingly (e.g., using a `src` directory for source files).
- **PlatformIO:** The library must be compilable using PlatformIO.
- **GitHub CI/CD:** A GitHub Actions workflow for continuous integration and deployment is required. The workflow must compile every version on every branch with every push (`on: [push, pull_request]`). This workflow must include a step to verify successful compilation with PlatformIO.
- **GitHub CI/CD (DE):** Ein GitHub-Actions-Workflow für die kontinuierliche Integration und Bereitstellung ist erforderlich. Der Workflow muss jede Version auf jedem Branch bei jedem Push kompilieren (`on: [push, pull_request]`). Dieser Workflow muss einen Schritt enthalten, um die erfolgreiche Kompilierung mit PlatformIO zu überprüfen.
