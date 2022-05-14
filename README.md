# OSSM-CUM(LUBE)-ADDON

![Final Addon](Assembled.jpg?raw=true "Housing Motor" )

Max Power Possible with Thick Mixture of X-Lube is 4L/min.

## Electrical Parts Needed:

| Quantity | Part | Sourcing EU | Price € |
|----------|------|-------------|---------|
| 1x | Nema 23 1,26Nm 2.8A 6,35mm Shaft | https://www.amazon.de/gp/product/B06XVC23Z7/ | 23.99 € | 
| 1x | TB6600 4A 9-42V Driver | https://www.amazon.de/gp/product/B095RNPBYN | 10,99 € |
| 1x | I2C Logik Level Converter | https://www.amazon.de/ARCELI-Converter-Bidirektionales-Shifter-Arduino/dp/B07RDHR315 | 0,6 € |
| 1x | LM7805 | https://www.amazon.de/St%C3%BCck-L7805-LM7805-7805-Spannungsregler-default/dp/B08Q8K5342 | 0,7 € |
| 1x | M5Stamp C3 |  https://www.robotshop.com/de/de/m5stamp-c3-mate-mit-stiftleisten.html | 8,26 € | 
| 1x | Soilder Breadboard | https://www.amazon.de/AZDelivery-Lochrasterplatte-Lochrasterplatine-Leiterplatte-Arduino/dp/B078HV79XX/ | 1 € |


## Connection Diagram 

C3 Pins to Level Shivter to Stepper driver 

| Stepper PIN | Level Shiverter Hi | Level Shivter LOW | C3 Stamp |
|-|-|-|-|
| - | 5V | - | 5V | 
| - | - | 3,3V | 3,3 V |
| Bride to all Pins | GND | GND | GND |
| PUL + | HV1 | LV1 | 4 |
| DIR + | HV2 | LV2 | 5 |
| ENA + | HV3 | LV3 | 6 | 

LM7805 Check Datasheet for Pinout normaly Top Sight from left to right is: 

| Input | Ground | Output |
|-|-|-|
| 12V Parrallel on Driver | GND Parrallel on Driver on C3 Stamp| 5V to C3 

## Hardware Needed

Nema 23 Peristaltic Pump comes from this Thingivers: https://www.thingiverse.com/thing:4797064

Parts to Buy 
| Quantity | Part | Sourcing EU | Price € |
|----------|------|-------------|---------|
| 9x | 625RS | https://www.amazon.de/Kugellager-Rillenkugellager-gummibeschichtetes-Metalllager-Hobby-Projekte/dp/B07TSP8SBP| 8 € |
| 3x | M5x25 Screw + Nut for Rotor | | | 
| 2x | M2 Screw | ||
| 2x | M2 Heat Set Nut | https://www.amazon.de/ruthex-Gewindeeinsatz-St%C3%BCck-Gewindebuchsen-Kunststoffteile/dp/B088QJG676/
| 4x | M5x45 Screw + Nut || 
| 1x | 9mm ID x OD 12mm Siliconetube food safe Qualtiy | https://www.amazon.de/gp/product/B07T12V62M/ | 10 € |

## 3D Printed Parts 

| Quantity | Part | Info | 3D Print Paramters 
|----------|------|---|---|
|1x | Housing_cover | | 6 Wall Layers 28% Infill PLA no Supports |
| 1x | Housing_motor_side or Housing Tubes UP | Depens on you mouting postion see picture | 6 Wall Layers 28% Infill PLA no Supports |
| 1x | Rotor | Depents on you shaft of stepper| 6 Wall Layers 28% Infill PLA With Supports |

![Housing Motor](Hardware/housing.jpg?raw=true "Housing Motor")

## Assembly

1. Heatset M2 Nut into Rotor Holes on Side 
2. Put Bearing in Rotor with M5x25 Screw and Nut Screw 3 Times 
3. Put Rotor on Stepper and Tighten with M2 Screws. 
4. Put Housing on over rotor to Stepper 
5. Put Tubing in between housing and Bearing 
6. Close Housing with M5 Screw

## Software

Software is right now in State with the OSSM M5 Remote https://github.com/ortlof/OSSM-M5-Remote
Clone the git in folder Cumpump and open Workspace with VS Code and Flash with plaformio.
Right now you need to Hard Code MAC Adresses.
