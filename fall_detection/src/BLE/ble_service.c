//  //  //  //  //  //  //  //  //  //  //  //
//  Tiresias Hearing Device Project         //
//  University of Sao Paulo                 //
//  Author: Joao Victor Colombari Carlet    //
//  //  //  //  //  //  //  //  //  //  //  //

#include "ble_service.h"

LOG_MODULE_REGISTER(BLE_MODULE, LOG_LEVEL_INF);

struct bt_conn *my_conn = NULL;

static const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
	(BT_LE_ADV_OPT_CONNECTABLE |
	 BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
	BT_GAP_ADV_FAST_INT_MIN_1, /* 0x30 units, 48 units, 30ms */
	BT_GAP_ADV_FAST_INT_MAX_1, /* 0x60 units, 96 units, 60ms */
	NULL
); /* Set to NULL for undirected advertising */

// data to be used in the advertising packet                        
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

// Data to be used in the scaning reponse packet  
static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL,
		      BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)),
};

// Callback connection functions 
void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection error %d", err);
        return;
    }
    LOG_INF("Connected");
    my_conn = bt_conn_ref(conn);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected. Reason %d", reason);
    bt_conn_unref(my_conn);
}

// Connection_callback structure 
struct bt_conn_cb connection_callbacks = {
    .connected              = on_connected,
    .disconnected           = on_disconnected,
};

int ble_service_init(void) {
	int err;

	LOG_INF("Starting BLE module\n");

	// Register our custom callbacks 
	bt_conn_cb_register(&connection_callbacks);

	LOG_INF("Callbacks okay\n");

	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return -1;
	}
	
	LOG_INF("Bluetooth initialized");
	
	err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return -1;
	}

	LOG_INF("Advertising successfully started");

	return -1;
}

int ble_service_thread(void)
{
	for (;;) {
		k_sleep(K_MSEC(500));
	}

	return -1;
}
