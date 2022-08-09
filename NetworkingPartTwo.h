#pragma once

void ConnectToPeers();

void ReceivePackets(ENetEvent& event);

void SendPackets(ENetEvent& event);
