XML-Beschreibungssprache    {#xmlsprache}
========================

\~German
Die Konfiguration der Simulationsszenarien erfolgt mit Hilfe einer speziellen XML-Sprache.
Für jedes Simulationsszenario legt der Benutzer eine XML-Datei an.
Diese XML-Datei kann dann vom Programm eingelesen und verarbeitet werden.

Beschreibungen der verschiedenen Objektgruppen:
-   [Elektrische Objekte](xmlelectrical.html)
-   [Parameterobjekte](xmlobject.html)
-   [Zustandsobjekte](xmlstate.html)
-   [Thermische Blöcke](xmlthermalblock.html)
-   [Kühlungen](xmlcooling.html)
-   [Thermische Materialien](xmlthermalmaterial.html)
-   [Optionen](xmloption.html)


Dazu:
-   Die Beschreibung der XML-Parametrierung und des Einlesevorgangs des [thermischen Modells](xmlthermalmodel.html), die die hier gezeigten XML-Strukturen ergänzen.
-   Die Besonderheiten beim [Benutzen des Simulink-Modells](xmlcinterface.html).

<br/>

Grundaufbau
===========

__Der Grundaufbau einer solchen Konfigurationsdatei:__
\htmlinclude xmlstructure_color.xml


__Die Konfigurationsdatei kann in drei Abschnitte unterteilt werden:__

-   **Options**<br/>
    In den [Options](xmloption.html) können globale Einstellungen vorgenommen werden.

-   **CustomDefinitions**<br/>
    In diesem Element können sogenannte Muster definiert werden.
    Später können diese Muster im [elektrischen Netzwerk](xmlelectrical.html) und im [thermischen Aufbau](xmlthermalblock.html) verwendet werden.

-   **RootElement**<br/>
    Das RootElement ist das Wurzelelement des elektrischen Netzwerkes.
      Über das Attribut "class" wird das assoziierte Objekt definiert.
      Meist ist dieses Objekt ein ParallelTwoport oder ein SerialTwoport.

Nur nötig, wenn das [thermische Modell](thermalmodel.xml) verwendet wird:
-   **ThermalMaterials**<br/>
    Enthält alle [Materialien](xmlthermalmaterial.html), die für das thermische Modell verwendet werden.

-   **CachedCoolings**<br/>
    Hier können die <**Cooling**>-Tags, die in den Kühlblöcken die Art und Stärke der Külung bestimmen, gecahcet werden.
  

-   **CoolingBlocks**<br/>
    Enthält [Kühlungen](xmlcooling.html), die für das thermische Modell verwendet werden.
    Diese werden hier erstellt, um an anderer Stelle per Objektreferenzierung übergeben zu werden.
    Also muss bei allen das Attribut cache="true" gesetzt werden.
    Kühlungen können auch innerhalb des RootElement definiert werden.



Das elektrische Netzwerk wird aus verschiedenen Objekten (Basisklasse TwoPort) aufgebaut.
Jedes Objekt wird in der Konfigurationsdatei durch ein XML-Element repräsentiert.
Die XML-Elemente besitzen, in Abhängigkeit des zu beschreibenen Objektes, unterschiedliche Attribute und Kindelemente.
Beispielsweise besitzt ein XML-Element für ein Objekt des Typs "ParallelTwoport" ein Kindelement "Children".
Dieses Element enthält seinerseits mehrere Kindelemente als Zweige der Parallelschaltung.
Eine Auflistung aller unterstützten elektrischen Objekttypen und die Beschreibung der jeweiligen XML-Elemente kann [hier nachgelesen werden](xmlelectrical.html).


__Neben den elektrischen Objekten werden auch weitere Objekttypen angeboten:__

-   **Zustandsobjekte (States)**<br/>
    Es existieren zwei unterschiedliche Zustandsobjekte:
    +   Soc <br/>
        Das Objekt Soc speichert den Ladezustand einer Batterie.
    +   ThermalState <br/>
        Das Objekt ThermalState (thermischer Zustand) speichert die Wärmeerzeugung durch Verlustleistung und die Temperatur eines elektrischen Bauteils.
<br/><br/>
-   **Parameterobjekte (Object)**<br/>
    Ein Parameterobjekt beschreibt Parameter wie beispielsweise den Widerstandswert eines Widerstands oder den Spannungswert einer Spannungsquelle.<br/>
    Diese Objektgruppe enthält verschiedene Arten von Parameterobjekten:
    +   Konstanter Parameterwert
    +   Parameterwert nach gegebener Funktion berechnet
    +   Nachschlagetabellen (abhängig von Soc oder Temperatur oder Soc und Temperatur)


Musterreferenzierung
====================

Für die Erstellung eines Musters wird das XML-Element im Abschnitt "CustomDefinitions" eingefügt.
Der Name des Elementes muss eindeutig sein.
Dannach kann das Muster an anderer Stelle (innerhalb "CustomDefinitions" und "RootElement") über das Attribut "ref" referenziert werden.

Hier ein Beispiel:
\htmlinclude basic_example_color.xml

Objektreferenzierung
============

Die XML-Beschreibungssprache erlaubt eine Objektreferenzierung.
Bei der Objektreferenzierung wird dasselbe Objekt an mehrere andere Objekte übergeben.
Das ist vor allem bei der Verwendung von Zustandsobjekten sinnvoll.

Damit ein Objekt referenziert werden kann, muss bei seiner Definition im XML-Element das Attribut "cache='True'" angegeben werden.
Dadurch wird das Objekt unter seinem Elementnamen im Cache zwischengespeichert.
An anderer Stelle kann dieses Objekt wieder über das Attribut "cacheref" referenziert werden.
Bei der erneuten Zwischenspeicherung eines Objektes gleichen Namens wird das vorherige Objekt im Cache überschrieben (nur der Zeiger auf dieses Objekt).

Hier ein Beispiel:
\htmlinclude xmlobject_ref_color.xml

In diesem Beispiel werden die beiden Zustandsobjekte "ThermalState" und "Soc" im Cache zwischengespeichert.
Die Parameterobjekte "LookupTau", "LookupOhmicResistance", "LookupPhi" verwenden diese Zustandsobjekte.
Im Gegensatz zu einer Musterreferenzierung werden bei der Objektreferenzierung keine neuen Objekte erzeugt.


Beispiel Konfigurationsdatei
============================

Ein einfaches Simulationsszenario für rein elektrische Simulation mit insgesamt 16 Batterien:
\htmlinclude testconfig_color.xml

\~English
The configuration of the simulation scenarios is done via a particular xml language.
For each scenario, a xml file has to be created by the user.
This xml file can be read and evaluated by the program.

Description of the different object groups:
-   [Electrical objects](xmlelectrical.html)
-   [Parameter objects](xmlobject.html)
-   [State objects](xmlstate.html)
-   [Thermal blocks](xmlthermalblock.html)
-   [Coolings](xmlcooling.html)
-   [Thermal materials](xmlthermalmaterial.html)
-   [Options](xmloption.html)


Furthermore:
-   The description of the xml parameterization and the scan process of the [thermal model](xmlthermalmodel.html), which complement the xml structure shown at this point.
-   The peculiarities in the [use of the Simulink-model](xmlcinterface.html).

<br/>

Basic structure
===========

__The base structure of a configuration file:__
\htmlinclude xmlstructure_color.xml


__The configuration file can be divided in three sections__

-   **Options**<br/>
    Global settings can be adjusted within the [options](xmloption.html)

-   **CustomDefinitions**<br/>
    Templates can be defined within this element.
    Afterwards these templates can be used within the [electrical network](xmlelectrical.html) and within the [thermal structure](xmlthermalblock.html).

-   **RootElement**<br/>
    The RootElement is the root node of the electrical network.
      The attribute "class" defines the corresponding object.
      In most cases this object is a ParallelTwoport or a SerialTwoport.

Only necessary if the [thermal model](thermalmodel.xml) is used:
-   **ThermalMaterials**<br/>
    Contains all [materials](xmlthermalmaterial.html), which are used for the thermal model.

-   **CachedCoolings**<br/>
    The <**Cooling**>-tags, which define the type and strength of the cooling within the cooling blocks, can be cached here.
  

-   **CoolingBlocks**<br/>
    Contains [coolings](xmlcooling.html), which are used for the thermal model.
    These are generated here, to be passed via object references afterwards.
    Therefore, the attribute cache="true" has to be set.
    Cooling can also be defined outside of the RootElement.



The electrical network is build with different objects (base class TwoPort).
Each object in the configuration file is represented by an xml element.
The xml elements contain, depending on the object, different attributes and child elements.
A xml element describing a "ParallelTwoport" , for example, contains a child element "Children".
This element for its part contains several child elements as branches of the parallel connection.
A listing of all supported electrical object and a description of each xml element can be found [here](xmlelectrical.html).


__ Besides the electrical objects there are several other possible object types:__

-   **State objects (States)**<br/>
   There are two different state objects:
    +   Soc <br/>
        The object Soc stores the state of charge of a battery
    +   ThermalState <br/>
        The object ThermalState stores the heat generation through power loss and the temperature of an electrical component.
<br/><br/>
-   **Parameter object (object)**<br/>
    A parameter object describes parameters, such as the resistance value of an ohmic resistor or the voltage value of a voltage source.<br/>
    This object group contains several types of parameter objects:
    +   Constant parameter values
    +   Parameter values evaluated according to given functions
    +   Lookup tables (depending on Soc or temperature or Soc and temperature)


Template referencing
====================

In order to create a template, the xml element is added to the section "CustomDefinitions".
The name of the element has to be unambiguously.
Afterwards, the template can be referenced at other points (within "CustomDefinitions" and "RootElement") via the attribute "cacheref".

An example:
\htmlinclude basic_example_color.xml

Object referencing
============

The xml language permits object referencing.
Object referencing is passing the same object multiple times to other object.
This is especially useful for the description of state objects.

The attribute "cache='true'" has to be used when defining an xml element in order to enable references.
Thus, the object is stored in the cache using its element name.
The object can be referenced via the cacheref attribute at any point.
On further caching of an object with the same name, the previous object will be overwritten (only the pointer).

An example:
\htmlinclude xmlobject_ref_color.xml

The state objects "ThermalState" and "Soc" are cached in this example.
The parameter objects "LookupTau", "LookupOhmicResistance", "LookupPhi" are using this state objects.
As opposed to referencing templates, no further objects will be created when referencing objects.


Example configuration file
============================

A simple simulation scenario with a pure electrical simulation with a total of 16 batteries:
\htmlinclude testconfig_color.xml


