/**
 * @file Fault.hpp
 * @brief Declaration of the Fault class.
 *
 * @version 0.0.1
 * @date 2024-07-31
 *
 * @author Aykan Yüce
 * @author Felix Uhle
 */

#pragma once

#include <cstdint>

#include "Util/Settings.hpp"

/**
 * @class Fault
 * @brief Faults are parameteriesed by a location (signal/position and time) and
 * a fault type.
 */
class Fault {

public:
  /**
   * @brief Constructor of Fault.
   *
   * @param signal_index The index of the signal/the position, which is faulted.
   * @param clock_cycle The clock cycle in which the Fault is injected.
   * @param fault_type The type of the Fault which is injected.
   */
  Fault(uint64_t signal_index, uint64_t clock_cycle,
        double fault_probability, FaultType fault_type);

  /**
   * @brief Accessor function for signal index of this fault.
   *
   * @return The index of this fault.
   */
  uint64_t GetSignalIndex() const;
  /**
   * @brief Accessor function for the target clock cycle of this fault.
   *
   * @return The clock cycle in which this fault should be injected.
   */
  uint64_t GetClockCycle() const;

  /**
   * @brief Accessor function for the probability with that this fault occurs.
   *
   * @return The probability with that this fault occurs.
   */
  // TODO: Should we store the prop of each fault or of each fault set?
  double GetFaultProbability() const;

  /**
   * @brief Accessor function for the type of the fault.
   *
   * @return The fault type, which should be injected.
   */
  FaultType GetFaultType() const;

  /**
   * @brief Computes the effect of this fault on a given fault free computation of a signal.
   *
   * @param fault_free_computation The fault free computation computed by the simulator.
   * @return The value of a signal faulted with this fault.
   */
  virtual uint64_t ComputeFaultEffect(uint64_t fault_free_computation) const = 0;

  /**
   * @brief Checks if this fault is applied to a given signal and clock cycle.
   *
   * @param signal_index Compared to the signal index of this fault.
   * @param clock_cycle Compared to the clock cycle of this fault.
   * @return Returns if this fault effects given clock cylce and signal.
   */
  bool IsSignalFaulted(uint64_t signal_index, uint64_t clock_cycle) const;

private:
  /**
   * @brief The index of the signal/the position, which should be faulted.
   */
  const uint64_t signal_index_;

  /**
   * @brief The clock cycle in which the fault should occur.
   */
  // TODO: Implement some complexety reductions,
  // which avoid to simulate the same fault twice.
  // E.g. Injecting a fault in a gate before the first register stage and
  // simulate this for the first and second clock. In both cases it will behave
  // the same if no feedback logic or disable logic is implemented. Furthermore,
  // we should only evalute false which are reachable, so faulting the third
  // logic stage in the first clock cycle makes no sense.
  const uint64_t clock_cycle_;

  /**
   * @brief The probability with that the fault is induced (required for random
   * fault model).
   */
  const double fault_probability_;

  /**
   * @brief The type of the fault.
   */
  const FaultType fault_type_;
};

