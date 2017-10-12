Zustandsobjekte     {#xmlstate}
===============
\~German
Es existieren zwei Arten von Zustandsobjekten: Ein thermischer Zustand (Temperatur und Wärmeerzeugung) und ein Ladezustand (SoC).
Die Zustandsobjekte werden im Zusammenhang mit einer Batterie verwendet.
Neben der Beschreibung des gegenwärtigen Zustandes einer Batterie, werden diese Zustände auch für [Parameterobjekte](xmlobject.html) mit Nachschlagetabellen verwendet.
Dadurch können die Parameter der elektrischen Objekte in Abhängigkeit der Batterietemperatur und des Ladezustandes berechnet werden.

<br/>

Allgemein
=========

Jedes XML-Element, das in der Konfiguration ein Zustandsobjekt repräsentiert, besitzt das Attribut "class".
Über dieses Attribut wird der Objekttyp zugeordnet.
Zustandsobjekte werden normalerweise im Cache zwischengespeichert und über die Objektreferenzierung mehrmals für Parameterobjekte mit Nachschlagetabellen verwendet.

<br/>

Grundtypen
==========

<br/>

Thermischer Zustand
----------

Dieser Zustand speichert die Wärmeerzeugung durch elektrische Verlustleistung und die Temperatur.

__Objekttyp: "ThermalState"__

<**InitialTemperature**>: Wert der anfänglichen Temperatur in °C

Definition eines thermischen Zustandes:
\htmlinclude thermalstate_color.xml


Ladezustand
----------

__Objekttyp: "Soc"__

<**MaxCapacity**>: Wert der maximalen Kapazität in Ah

<**InitialSoc**>: Wert des anfänglichen Ladezustandes in Prozent (0 bis 100)

<**MeasurementPoints**>: Messpunkte zur Korrelation von Soc und Kapazität. Wird derzeit nicht verwendet. Muss aber angegeben werden.

Definition eines Ladezustandes:
\htmlinclude socstate_color.xml

Strom, Spannung
----------

__Object type: "ElectricalState"__

Zusätzliche Attribut:

**StateType='Current'**

oder

**StateType='Voltage'**

Strom und Spannung der elektrischen Komponenten können als weiterer State genutzt werden. Damit kann z.B. das Verhalten der Butler–Volmer Gleichung nachgestellt werden.

\htmlinclude xmlCurrentState_color.xml


\~English
There are two different types of state objects: A thermal state (temperature and heat generation) and a state of charge (SoC).
The state objects are used in connection with a battery.
Apart from the description of the current battery condition, these states are also used for [parameter objects](xmlobject.html) with lookup tables.
As a result, the parameters of the electrical object could be calculated as a function of battery temperature and SoC.

<br/>

Abstract
=========

Each xml element, which represents a state object in the configuration, possesses the attribute "class".
The object type is defined by this attribute.
State objects are normally stored in the cache and are used multiple times via object references for parameter objects with lookup tables.

<br/>

Basic types
==========

<br/>

Thermal state
----------

This state stores the heat generation by electrical power loss and the temperature.

__Object type: "ThermalState"__

<**InitialTemperature**>: Value of the initial temperature °C

Definition of a thermal state:
\htmlinclude thermalstate_color.xml


State of charge
----------

__Object type: "Soc"__

<**MaxCapacity**>: Value of the maximal capacity in Ah

<**InitialSoc**>: Value of the initial charge in percent (0 bis 100)

<**MeasurementPoints**>: measurement points for correlation of SoC and capacity. Currently not in use, but has to be declared.

Definition of a state of charge:
\htmlinclude socstate_color.xml


Current, Voltage
----------

__Object type: "ElectricalState"__

Additional attributes:

**StateType='Current'**

or

**StateType='Voltage'**

The current and voltage of an electric component can be used as an input for lookups. Therefore non-linear behavior e.g. for the Butler–Volmer equation can be approximated. 

\htmlinclude xmlCurrentState_color.xml
