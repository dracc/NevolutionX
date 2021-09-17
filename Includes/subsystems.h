#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H

#include "config.hpp"

#ifdef NXDK
void mountHomeDir(const char Letter);
#endif
int init_systems(const Config &config);
void shutdown_systems(int systems);

#endif
