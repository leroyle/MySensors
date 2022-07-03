/*
* The MySensors Arduino library handles the wireless radio link and protocol
* between your home built sensors/actuators and HA controller of choice.
* The sensors forms a self healing radio network with optional repeaters. Each
* repeater and gateway builds a routing tables in EEPROM which keeps track of the
* network topology allowing messages to be routed to nodes.
*
* Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
* Copyright (C) 2013-2017 Sensnology AB
* Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
*
* Documentation: http://www.mysensors.org
* Support Forum: http://forum.mysensors.org
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*/

// Topic structure: MY_MQTT_PUBLISH_TOPIC_PREFIX/NODE-ID/SENSOR-ID/CMD-TYPE/ACK-FLAG/SUB-TYPE
// #include "MyConfig.h"
// #include "MyProtocol.h"

#include "MyGatewayTransport.h"
// global variables
extern MyMessage _msgTmp;

// #include "MyMessage.h"
// #include "MyProtocol.h"

// #if defined MY_HTTP_SERVER_IP_ADDRESS
// IPAddress _httpServer(MY_CONTROLLER_IP_ADDRESS);
// #endif

// #if defined(MY_IP_ADDRESS)
// IPAddress _httpGatewayIP(MY_IP_ADDRESS);
// #if defined(MY_IP_GATEWAY_ADDRESS)
// IPAddress _gatewayIp(MY_IP_GATEWAY_ADDRESS);
// #elif defined(MY_GATEWAY_ESP8266) /* Elif part of MY_IP_GATEWAY_ADDRESS */
// // Assume the gateway will be the machine on the same network as the local IP
// // but with last octet being '1'
// IPAddress _gatewayIp(_HTTP_serverIp[0], _HTTP_serverIp[1], _HTTP_serverIp[2], 1);
// #endif /* End of MY_IP_GATEWAY_ADDRESS */
// #if defined(MY_IP_SUBNET_ADDRESS)
// IPAddress _subnetIp(MY_IP_SUBNET_ADDRESS);
// #elif defined(MY_GATEWAY_ESP8266) /* Elif part of MY_IP_SUBNET_ADDRESS */
// IPAddress _subnetIp(255, 255, 255, 0);
// #endif /* End of MY_IP_SUBNET_ADDRESS */
// #endif /* End of MY_IP_ADDRESS */

static bool _Custom_connecting = true;
static bool _Custom_available = false;
static MyMessage _Custom_msg;

bool gatewayTransportSend(MyMessage &message)
{
	GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPS:Send...\n"));
	if (Custom_userTransportSend)
	{
		GATEWAY_DEBUG(PSTR("Calling user transport Send\r\n"));
		return Custom_userTransportSend(message);
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPS: API error: user supplied Custom_userTransportSend() is not defined\n"));
		GATEWAY_DEBUG(PSTR("Custom Send user API not found"));
	}

	return false;
}

void incomingCustom(char *topic, uint8_t *payload, unsigned int length)
{
}

bool reconnectCustom(void)
{
	return false;
}

bool gatewayTransportConnect(void)
{
	GATEWAY_DEBUG(PSTR("GWT:CUSTOM:CONNECTING...\n"));
	if (Custom_userTransportConnect)
	{
		return Custom_userTransportConnect();
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:Custom:TPC: API error: user supplied Custom_userTransportConnect() is not defined\n"));
	}
	return false;
}

bool gatewayTransportInit(void)
{
	GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPI:INIT...\n"));
	bool transInit = false;

	if (Custom_userTransportInit)
	{

		transInit = Custom_userTransportInit();
		if (!transInit)
		{
			GATEWAY_DEBUG(PSTR("TransportInit: failed"));
			return false;
		}
		GATEWAY_DEBUG(PSTR("TransportInit: success\r\n"));
		gatewayTransportConnect();

		(void)gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
		// Send presentation of locally attached sensors (and node if applicable)
		presentNode();
		return true;
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPI: API error: user supplied Custom_userTransportInit() is not defined\n"));
		return false;
	}

	_Custom_connecting = false;
	return true;
}

bool gatewayTransportAvailable(void)
{
	//GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPA:Available...\n"));
	if (Custom_userTransportAvailable)
	{
		return Custom_userTransportAvailable();
	}
		else
	{
		GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPA: API error: user supplied Custom_userTransportAvailable() is not defined\n"));
		return false;
	}

	Serial.println("************ ERROR  ************ FIX THIS ***********");

	return false; //_Custom_available;
}

MyMessage &gatewayTransportReceive(void)
{
	GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPR:Receive...\n"));
	if (Custom_userTransportReceive)
	{
		return Custom_userTransportReceive();
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:CUSTOM:TPR: API error: user supplied Custom_userTransportReceive() is not defined\n"));
		_Custom_available = false;
		return _Custom_msg;
	}

	Serial.println("************ ERROR  ************ FIX THIS ***********");
	// Return the last parsed message
	_Custom_available = false;
	return _Custom_msg;
}
