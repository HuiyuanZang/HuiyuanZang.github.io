# Introduction to the Arm Architecture

The Arm architecture described in this Architecture Reference Manual defines the behavior of an abstract machine, referred to as a processing element, often abbreviated to PE.

Arm defines three architecture profiles

## A Application profile

- Supports a Virtual Memory System Architecture(VMSA) based on a Memory Management Unit(MMU)
- Suppports the A64, A32, and T32 instrauction sets

## R Real-time profile

- Supports a Proteted Memory System Architecture(PMSA) based on a Memory Protection Unit(MPU)
- Supports an optional VMSA based on an MMU.
- Supports the A64,A32, and T32 instrcution sets.

## M Microcontroller profile

- Implements a programmers' model designed for low-latency interrupt processing, with hardware stacking of registers and support for writing interrupt handlers in high-level languages.
- Implements a variant of the R-profile PMSA.
- Supports a variant of the T32 instrauction set

# Arm architecural concepts

## Exectution State

- The supported register widths
- The supported instruction sets
- Significant aspects of
  - The Exeption model
  - The Virtual Memory System Architecture
  - The programmers' model

## The instruction sets

## System Registers

## Arm Debug
