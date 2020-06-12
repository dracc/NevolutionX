#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H

int init_systems();
int init_input();
int init_drives();
int init_video();
int init_network(void *);
void shutdown_systems(int systems);

#endif
