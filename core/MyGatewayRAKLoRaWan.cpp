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


#include "MyGatewayTransport.h"

static bool _LoRaWan_connecting = true;
static bool _LoRaWan_available = false;
static MyMessage _LoRaWan_msg;

bool gatewayTransportSend(MyMessage& message)
{
	GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPS:Send...\n"));
	if(LoRaWan_userTransportSend)
	{
		return LoRaWan_userTransportSend(message);
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPS: API error: user supplied LoRaWan_userTransportSend() is not defined\n"));
	}	

	return false;
}

void incomingLoRaWan(char* topic, uint8_t* payload, unsigned int length)
{

}

bool reconnectLoRaWan(void)
{

}

bool gatewayTransportConnect(void)
{
	GATEWAY_DEBUG(PSTR("GWT:LoRaWan:CONNECTING...\n"));
	if(LoRaWan_userTransportConnect)
	{
		return LoRaWan_userTransportConnect();
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPC: API error: user supplied LoRaWan_userTransportConnect() is not defined\n"));
	}
	return false;
}

bool gatewayTransportInit(void)
{
	GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPI:INIT...\n"));
	bool transInit = false;

	if(LoRaWan_userTransportInit)
	{

		transInit = LoRaWan_userTransportInit();
		if (!transInit)
		{
			GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TransportInit: failed"));
			return false;
		}
		GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TransportInit: success"));
		gatewayTransportConnect();

		(void)gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
	        // Send presentation of locally attached sensors (and node if applicable)
		presentNode();
		return true;
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPI: API error: user supplied LoRaWan_userTransportInit() is not defined\n"));
		return false;
	}


	_LoRaWan_connecting = false;
	return true;
}

bool gatewayTransportAvailable(void)
{
	//GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPA:Available...\n"));
	if(LoRaWan_userTransportAvailable)
	{
		return LoRaWan_userTransportAvailable();
	
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPA: API error: user supplied LoRaWan_userTransportAvailable() is not defined\n"));
		return false;
	}

	Serial.println("************ ERROR  ************ FIX THIS ***********");

	return false; //_LoRaWan_available;
}

MyMessage & gatewayTransportReceive(void)
{
	GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPR:Receive...\n"));
	if(LoRaWan_userTransportReceive)
	{
		return LoRaWan_userTransportReceive();
	}
	else
	{
		GATEWAY_DEBUG(PSTR("GWT:LoRaWan:TPR: API error: user supplied LoRaWan_userTransportReceive() is not defined\n"));
		_LoRaWan_available = false;
		return _LoRaWan_msg;
	}

	Serial.println("************ ERROR  ************ FIX THIS ***********");
	// Return the last parsed message
	_LoRaWan_available = false;
	return _LoRaWan_msg;
}
