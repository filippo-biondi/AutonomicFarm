#pragma once


/**
 * Enum that represents the possible actions that can be taken by the manager.
 * Actions are decided by the Analyzer and used by the Planner and the Executor.
 */
enum Action
{
	INCREASE_WORKER,
	DECREASE_WORKER,
	RECOVER_QUEUE,
	NO_ACTION,
};