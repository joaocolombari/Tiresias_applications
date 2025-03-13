//  //  //  //  //  //  //  //  //  //  //  //
//  Tiresias Hearing Device Project         //
//  University of Sao Paulo                 //
//  Author: Joao Victor Colombari Carlet    //
//  //  //  //  //  //  //  //  //  //  //  //

#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>

// Macros
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/* Declaracao do semaforo */
extern struct k_sem fall_sem;

/* Declaracoes das funcoes */

void on_connected(struct bt_conn *conn, uint8_t err);

void on_disconnected(struct bt_conn *conn, uint8_t reason);

int ble_service_init(void);

int ble_service_thread(void);

#endif