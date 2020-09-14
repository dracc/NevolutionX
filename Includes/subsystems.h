#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H

#ifdef NXDK
void mountHomeDir(const char Letter);
#endif
int init_systems();
void shutdown_systems(int systems);

#endif
