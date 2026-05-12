# SecureCarry
### An Intelligent IoT-Based Bag Security System

**Authors:**  
Aishwarya S  
Harini S  
M. S. Lakshita  

SecureCarry is an IoT-based smart bag security system designed to protect valuable belongings using multi-factor authentication, GPS tracking, and environmental monitoring. The system transforms a normal bag into an intelligent security device capable of detecting unauthorized access, tracking location, and sending real-time alerts to the owner.

---

# Project Overview

In today’s world, bags often carry valuable items such as laptops, research documents, electronic gadgets, and confidential files. Traditional bags provide minimal protection and are vulnerable to theft, tampering, or accidental damage such as water spills.

SecureCarry addresses this problem by integrating embedded hardware, sensors, and IoT communication into a compact module that can be mounted inside a bag. The system continuously monitors security conditions and alerts the user instantly through mobile notifications.

The project combines embedded systems, IoT communication, and sensor monitoring to provide an affordable and practical bag security solution.

---

# Features

## Two-Factor Authentication
SecureCarry uses layered authentication for access:

1. RFID card authentication
2. 4-digit PIN entry using keypad

Both conditions must be satisfied for the pouch to unlock.

## Smart Lock Mechanism
A micro servo motor controls the pouch lock and automatically relocks after a fixed time.

## Security Lockdown
If three incorrect authentication attempts occur:

- System enters a 5-minute lockdown
- All access attempts are rejected
- Telegram alert is sent
- GPS location of the bag is shared

## Emergency Override
A physical emergency button allows immediate unlocking of the pouch in urgent situations while sending a security alert.

## Water Detection
A water sensor monitors moisture levels inside the bag. If a spill is detected:

- Buzzer alert is triggered
- Telegram notification is sent

## GPS Location Tracking
The system can transmit location information during security events or when requested through Telegram commands.

---

# Hardware Components

| Component | Quantity | Purpose |
|---|---|---|
| ESP32 NodeMCU | 1 | Main controller with WiFi |
| RFID Module (RC522) | 1 | User authentication |
| RFID Tags/Cards | 2 | Authorized user ID |
| 4x3 Keypad | 1 | PIN entry |
| Micro Servo (SG90) | 1 | Lock mechanism |
| Water Sensor | 1 | Moisture detection |
| GPS Module (NEO-6M) | 1 | Location tracking |
| Li-ion 18650 Battery | 1 | Power supply |
| TP4056 Charging Module | 1 | Battery charging |
| Wires, PCB, Connectors | Set | Circuit assembly |
| Enclosure | 1 | System housing |

Estimated cost of the system is approximately ₹1460.

---

# System Workflow
RFID Scan
↓
RFID Authentication
↓
PIN Entry via Keypad
↓
Access Granted
↓
Servo Unlocks Pouch
↓
Telegram Notification Sent

---


If authentication fails multiple times, the system automatically activates security lockdown.

---

# Telegram Commands

| Command | Function |
|---|---|
| /lock | Activate manual lockdown |
| /unlock | Cancel lockdown |
| /location | Send current GPS coordinates |

---

# Test Cases

| Test Case | Expected Outcome |
|---|---|
| Valid RFID + correct PIN | Pouch unlocks and Telegram alert sent |
| Valid RFID + wrong PIN (3 attempts) | Lockdown activated with alert |
| Unauthorized RFID scans (3 attempts) | Lockdown triggered |
| Emergency button pressed | Immediate unlock + GPS location sent |
| Water detected | Buzzer alert and Telegram notification |
| /lock command | Manual lockdown activated |
| /unlock command | Lockdown removed |
| /location command | GPS coordinates sent |

---

# Applications

## Travel Security
Protects passports, laptops, and electronic devices in crowded environments such as airports and stations.

## Student Bags
Secures laptops, tablets, and academic materials.

## Healthcare
Ensures safe transport of medicines and medical kits.

## Field Research
Protects expensive equipment during outdoor research.

## Logistics and Delivery
Tracks parcel access and detects tampering during transport.

---

# Future Improvements

The system can be further improved with:

- Fingerprint authentication
- GSM module for areas without WiFi
- Dedicated mobile application
- Battery level monitoring and alerts
- Custom PCB design for compact integration

---

# Demo Video

Project demonstration:  
https://go.screenpal.com/watch/cOfQDtnT94a

---

# License

This project is released under the MIT License.

---

# Acknowledgements

This project was developed as part of an embedded systems and IoT learning initiative to explore practical applications of smart security systems.
