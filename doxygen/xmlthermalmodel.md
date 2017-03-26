Einlesen des thermischen Modells  {#xmlthermalmodel}
===================

\~German
Allgemein
=========

Die Parametrierung des thermischen Modells geschieht durch eine XML-Datei.
Ziel ist es hierbei, regelmäßige Grundstrukturen aufzubauen, in denen bei Bedarf aber auch einzelne, unregelmäßige Strukturen hinzugefügt werden können.
Grundidee des entwickelten Konzepts ist es, die XML-Datei zweimal (jeweils für das elektrische und das thermische Modell) in derselben Reihenfolge durchzuparsen.
Dadurch kann an Hand derselben Reihenfolge die Zuordnung zwischen beiden Modellen vorgenommen werden.
Basis des thermischen Modells sind die thermischen Blöcke und Kühlungen.
Die thermischen Blöcke definieren jeweils alle wichtigen Daten, die in einem bestimmten Raum gegeben sind.
Die [Kühlungen](xmlcooling.html), [thermischen Materialien](xmlthermalmaterial.html) und [thermischen Blöcke](xmlthermalblock.html) werden an anderer Stelle mitsamt Parametrierung genauer erläutert.
Jede Oberfläche, die zur Umwelt offen ist, wird mit der Standardkühlung beaufschlagt, diese kann in den Optionen eingestellt werden.
[Globale Optionen](xmloption.html) können im <**Options**>-Knoten eingestellt werden.


Grundlegendes zur Software
=========
Der Ablauf der Software des thermischen Modells soll im Folgenden kurz umrissen werden (siehe Bild):
- Die Parametrierung des thermischen Modells ist in der XML-Datei gegeben und in die Parametrierung des elektrischen Modells eingewoben.
- Der Hauptteil der Parametrierung erfolgt durch Definition von thermischen Blöcken und Kühlungen, die in der XML-Datei gegeben sind.
- Diese Erzeugen die folgenden Datenstrukturen und übergeben diese an die Thermische-Modell-Klasse:
  - Thermische Blöcke: Finite Volumen, Wärmeleitung, Außenflächen und Geometrische Ausdehung
  - Kühlungen: Geometrie, Außenflächen und Stärke der Kühlung
- Die Thermische-Modell-Klasse erzeugt blanke Datenstrukturen, die dem Gleichungssystem übergeben werden.
- Das Gleichungssystem wird einmal zu Anfang zusammengebaut. Danach beginnt die eigentliche Simulation: Das Gleichungssystem übergibt dem Gleichungslöser alle benötigten Daten. Dies wiederholt sich, bis die Simulationszeit vollständig durchsimuliert ist.

<br/><br/>
![Darstellung des Ablaufs der Software des thermischen Modells](ThermischesModellOutline.png)





Einlesevorgang
=========
Zuerst werden nacheinander folgende Knoten geparst:
- <**ThermalMaterials**>: Hier sind alle thermischen Materialien definiert, die im Modell benutzt werden.
  Jedes Material muss mit cache="true" gespeichert und später mit cacheref aufgerufen werden.
- <**CachedCoolings**>: Hier können physikalischen Komponenten der Kühlungen definiert werden.
  Diese müssen mit cache="true" gespeichert und später mit cacheref aufgerufen werden.
  Dies geschieht, indem in den Kühlblöcken der <**Cooling**>-Knoten mit cachref-Attribut aufgerufen wird.
  Die physikalischen Komponenten der Kühlungen können auch direkt in den <**CoolingBlocks**>-Knoten definiert werden, wobei der <**Cooling**>-Knoten statt gecachrefet zu werden an Ort und Stelle definiert wird.
- <**CoolingBlocks**>: Hier können Kühlungen definiert werden.
  Diese müssen mit cache="true" gespeichert und später mit cacheref aufgerufen werden.
  Jede Kühlung kann optional mit einer count-Schleife (dx, dy, dz, siehe nächsten Abschnitt) vervielfacht werden; hierfür müssen die entsprechenden Attribute im xml-Knoten angegeben werden.
  Kühlungen können auch in den <**CoolingBlocks**>-Knoten, die im RootElement rekursiv enthalten sind, definiert werden.

Dann beginnt der eigentliche Parsevorgang.
Der Parser des thermischen Modells parst rekursiv durch die XML-Knoten, bis er
- in einem Zellelement oder einem ohmschen Widerstand den <**ThermalBlock**>-, <**AdditionalBlocks**> oder <**CoolingBlocks**>-Knoten antrifft oder
- in einer Serien- oder Parallelschaltung den <**AdditionalBlocks**> oder <**CoolingBlocks**>-Knoten antrifft.
Wenn dies passiert, werden entsprechend den gegeben Anweisungen thermische Blöcke oder Kühlungen erstellt:

\htmlinclude thermaleinlesen_color.xml

- Ein <**ThermalBlock**>-Knoten führt direkt zur Erstellung eines thermischen Blocks, der mit dem elektrischen Modell gekoppelt ist, also Wärme und Temperatur mit einem elektrischen Element austauscht.
- Ein <**AdditionalBlocks**>-Knoten beinhaltet eine Auflistung von thermischen Blöcken, die nicht mit dem elektrischen Modell gekoppelt sind.
- Ein <**CoolingBlocks**>-Knoten beinhaltet eine Auflistung von Kühlungen.

Platzierung der thermischen Blöcke
=========
Die Platzierung der thermischen Blöcke wird durch die Attribute "dx", "dy" und "dz" gesteuert.
Diese Attribute können an drei verschiedenen Stellen auftauchen.
- In den <**Children**>-Knoten der Serien- und Parallelschaltungen
- In den Kinder-Knoten der <**Children**>-Knoten der Serien- und Parallelschaltungen
- In den Kinder-Knoten des <**AdditionalBlocks**>-Knotens

Sie definieren Verschiebungen im globalen kartesischen Koordinatensystem.
Es wird, beginnend vom globalen Ursprung, der Platzierungspunkt jeweils um den entsprechenden Wert verschoben und dieser verschobene Platzierungspunkt rekursiv weitergereicht, wo er wiederum verschoben wird, usw.
Die genaue Systematik ist wie folgt:
- Der <**Children**>-Knoten wird "count"-Attribut-mal durchgeparst. Für jeden Durchgang nach dem ersten wird der Platzierungspunkt um die Werte in den Attributen "dx", "dy" und "dz" verschoben.
- In den Kinder-Knoten des <**Children**>-Knotens und des <**AdditionalBlocks**>-Knotens kann zusätzlich noch eine Verschiebung durch die Attribute "dx", "dy" und "dz" erfolgen.

Platzierung von thermischen Sensoren
=========
Über den Knoten <**ThermalProbe**> können Sensoren beliebig im Raum eingetragen werden. Im Unterknoten <**Position**> können diese in X,Y,Z; Tuppeln hinterlegt werden.
Die Sensoren geben suchen sich den nächste Mittelpunkt eines finiten Volumens und geben dann die Temperatur des Volumens aus.
Nützlich kann diese Funktion sein, um vermutete Hotspot genauer zu betrachten oder die Messstellen in einem realen Pack nachzuempfinden.

\htmlinclude xmlThermalProbe_color.xml


\~English
Abstract
=========

The parametrization of the thermal model is done by the use of a xml file.
The aim is to construct a regular base structure, which could be expanded on demand to include single irregular structures.
The basic idea of the developed concept is to parse the xml file twice in the same order (once for the electrical and once for the thermal model).
Thereby the connection between both models is done by identical structural order.
The basis of the thermal model are the thermal blocks and the cooling.
The thermal blocks define all important data, which are given in a particular space.
The [cooling](xmlcooling.html), [thermal materials](xmlthermalmaterial.html) and [thermal blocks](xmlthermalblock.html) are addressed in more detail in other context.
Standard cooling is applied to each surface, which is open to the environment.This can be adjusted in the options.
[Global options](xmloption.html) can be adjusted in the <**Options**>-node.


Sofware basics
=========
The software workflow of the thermal model should be shortly outlined in the following paragraph (see picture):
- The parameterization of the thermal model is given in the xml file and is woven into the parameterization of the electrical model.
- The major part of the parameterization is given by the definition of thermal blocks and coolings, which are supplied by the xml file.
- This creates the following data structures and passes them to the thermal-model class:
  - Thermal blocks: finite volumes, thermal conductivity, outer surface and geometrical dimensions
  - Cooling: geometry, outer surfaces, strength
- The thermal-model class generates a blank data structure, which is passed to the equation system.
- The equation system is constructed once at the beginning. Afterwards, the actual simulation starts: The equation system passes all necessary data to the solver. This process is repeated until the simulation time is entirely simulated.

<br/><br/>
![Depiction of the software workflow of the thermal model](ThermischesModellOutline.png)





Scanning process
=========
Firstly, following nodes are parsed consecutively:
- <**ThermalMaterials**>: All thermal materials, which are in use, are defined here.
  Every material has to be cached with cache="true" and afterwards called with cachref.
- <**CachedCoolings**>: All physical components of the cooling can be defined here.
  Those have to be cached with cache="true" and afterwards called with cachref.
  This is done by calling the <**Cooling**>-node within the cooling block with the cachref-attribute.
  The physical component of the cooling can also be defined directly within the <**CoolingBlocks**>-node. Hereby the <**CoolingBlocks**>-node is defined in sito.
- <**CoolingBlocks**>: The coolings can be defined here.
  These have to be cached with cache="true" and afterwards called with cachref.
  Every cooling can optionally be multiplied with a count-loop(dx, dy, dz, see next section); therefore, the corresponding attributes in xml nodes have to be given.
  Coolings can also be defined in the <**CoolingBlocks**>-nodes, which are recursively contained in the RootElement.

Afterwards, the actual parse process begins.
The parser of the thermal model parses the xml nodes recursively, until
- he comes across a <**ThermalBlock**>-, <**AdditionalBlocks**> or <**CoolingBlocks**>-node within a cell element or an ohmic resistance, or
- he comes across <**AdditionalBlocks**> or <**CoolingBlocks**>-nodes within a series or parallel connection.
If this occurs, thermal blocks or coolings are created according to the given definitions:

\htmlinclude thermaleinlesen_color.xml

- A <**ThermalBlock**>-node leads directly to the creation of a thermal block, which is linked with the electrical model. Consequently, its heat and temperature are interchanged with the elecrical model.
- An <**AdditionalBlocks**>-node contains a listing of thermal blocks, which are not linked with the electrical model.
- A <**CoolingBlocks**>-node contains a listing of coolings.

Positioning of the thermal blocks
=========
The position of the thermal blocks is controlled by the attributes "dx", "dy" and "dz".
Those attributes can appear at three different places.
- Within the <**Children**>-nodes of the series or parallel connections.
- Wihtin the child nodes of the  <**Children**>-nodes of the series or parallel connections.
- Within the child nodes of the <**AdditionalBlocks**>-nodes

They define the shift in the global cartesian coordinate system.
Starting at the global origing, the placement point is shifted in each case by the corresponding value. This placement point is then passed recursively.
The exact system is as follows:
- The <**Children**>-node is parsed "count" times. For each repetition the placement is shifted by the values of the attributes "dx", "dy" and "dz".
- A further shift by "dx", "dy" and "dz" within the child nodes of the <**Children**>-node and the <**AdditionalBlocks**>-node is possible.

Positioning of thermal sensors
=========
By the use of <**ThermalProbe**>-nodes, sensors can be added at arbitrary point. They can be deposited as X,Y,Z tuples in the subnode <**Position**>.
The sensors compute the nearest centre of a finite volume and output the volumes temperature.
This function can be useful to examine suspected hotspots in detail or recreate measuring points in real packs.

\htmlinclude xmlThermalProbe_color.xml


