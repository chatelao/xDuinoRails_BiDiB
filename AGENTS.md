# Agent Guidelines

This file contains instructions for AI agents working on this repository.

## Development Process

Before starting any implementation work related to the BiDiB Arduino library, you **must** consult the `IMPLEMENTATION_PLAN.md` document. This document outlines the agreed-upon development phases and the logical order of implementation.

After completing a development phase or a significant feature, you **must** update the `IMPLEMENTATION_PLAN.md` to reflect the current status of the project.

## Entwicklungsprozess

Vor jeder Umsetzung ist das `IMPLEMENTATION_PLAN.md` zu lesen. Nach der Umsetzung ist dasselbe Dokument nachzuführen.

## Documentation Standards

- **File Naming:** Markdown files (`.md`) must use UPPER_SNAKE_CASE filenames (e.g., `README.md`, `IMPLEMENTATION_PLAN.md`).

## Dokumentationsstandards (DE)

- **Dateibenennung:** Markdown-Dateien (`.md`) müssen UPPER_SNAKE_CASE-Dateinamen verwenden (z. B. `README.md`, `IMPLEMENTATION_PLAN.md`).

## Project Structure and CI/CD

- **Arduino Library:** This project is planned as an Arduino library. The code should be structured accordingly (e.g., using a `src` directory for source files).
- **PlatformIO:** The library must be compilable using PlatformIO.
- **GitHub CI/CD:** A GitHub Actions workflow for continuous integration and deployment is required. The workflow must compile every version on every branch with every push (`on: [push, pull_request]`). This workflow must include a step to verify successful compilation with PlatformIO.
- **GitHub CI/CD (DE):** Ein GitHub-Actions-Workflow für die kontinuierliche Integration und Bereitstellung ist erforderlich. Der Workflow muss jede Version auf jedem Branch bei jedem Push kompilieren (`on: [push, pull_request]`). Dieser Workflow muss einen Schritt enthalten, um die erfolgreiche Kompilierung mit PlatformIO zu überprüfen.

## Testing

- **Comprehensive Testing:** For every new function, both unit tests and end-to-end transmit/receive (TX/RX) tests must be written. This ensures that each component works correctly in isolation and integrates properly within the overall communication flow.
- **Umfassende Tests (DE):** Für jede neue Funktion müssen sowohl Unit-Tests als auch End-to-End-Sende/Empfangs-Tests (TX/RX) geschrieben werden. Dadurch wird sichergestellt, dass jede Komponente isoliert korrekt funktioniert und sich ordnungsgemäß in den gesamten Kommunikationsfluss integriert.

## Code Formatting

- **Constant Alignment:** When defining multiple constants in a row, the `=` signs should be vertically aligned. Additionally, the numeric values should be right-aligned based on the widest value in the block. The original numeric base (e.g., decimal, hexadecimal) of all values must be preserved.

- **Konstantenausrichtung (DE):** Bei der Definition von mehreren Konstanten hintereinander sollen die `=` Zeichen vertikal ausgerichtet werden. Zusätzlich sollen die numerischen Werte rechtsbündig am breitesten Wert im Block ausgerichtet werden. Die ursprüngliche numerische Basis (z.B. dezimal, hexadezimal) aller Werte muss erhalten bleiben.
