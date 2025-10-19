# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the cognitive kernel subsystem
#

obj-$(CONFIG_COGNITIVE_KERNEL) += cognitive.o

cognitive-y := core.o
cognitive-$(CONFIG_COGNITIVE_ATOMSPACE) += atomspace.o
cognitive-$(CONFIG_COGNITIVE_SCHEDULER) += sched.o
cognitive-$(CONFIG_COGNITIVE_MEMORY) += memory.o
cognitive-$(CONFIG_COGNITIVE_SYSCALLS) += syscalls.o
cognitive-$(CONFIG_COGNITIVE_DEBUG) += debug.o