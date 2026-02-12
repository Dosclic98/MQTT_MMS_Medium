# Attack Graph driven Discrete Event Simulation for security assessment in Power Systems — Simulation Model - SIGSIM-PADS 2026

This repository contains the OMNeT++ simulation model executed by the experiments in the [`dbn-sim-learning`](https://github.com/Dosclic98/dbn-sim-learning) project.

## Compatibility

Developed and tested with:

- **OMNeT++**: 6.0.3
- **INET**: 4.5.0
- **Simu5G**: 1.2.2

## Model executed in the paper

The experiments execute the `IncrementalTest` network using the configuration in `simulations/omnetpp_new.ini`.

Key entry points:

- `simulations/IncrementalTest.ned`: topology and module composition (includes the `AttackGraph` submodule)
- `simulations/omnetpp_new.ini`: parameters used by the experiment runs
- `simulations/aggregator.sh`: aggregates per-run logs/traces into an aggregated CSV used downstream

## Where to find the Attack Graph files

- **Attack Graph implementation (C++/NED)**: `src/graph/attack/` (e.g., `AttackGraph.*`, `AttackNode.*`)
- **Paper scenario instance (JSON)**: `simulations/attackGraph.json`
- **Attack-to-controller mapping (JSON)**: `simulations/controllerMappings.json`

## Where to find the Control Finite State Machine (CFSM) implementation

- **FSM framework + states**: `src/fsm/`
- **Controller-side FSM integration (factories, state/transition wiring)**: `src/controller/fsm/`
- **Controllers using the CFSMs (examples)**: `src/controller/server/`, `src/controller/client/`, `src/controller/attacker/`
