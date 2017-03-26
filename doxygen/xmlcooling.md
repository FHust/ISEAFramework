Kühlungen     {#xmlcooling}
===================

\~German

Nachfolgend sind alle Kühlungen, die im thermischen Modell verwendet werden können, aufgelistet.
Für jede Kühlung wird der Aufbau des dazugehörigen XML-Elementes beschrieben und ein Codebeispiel gegeben.

<br/>

Allgemein
=========
Jede Kühlung hat einen geometrischen Teil (Lage im Raum) und einen physikalischen Teil (Parametrierung der Stärke der Kühlung).
Der geometrische Teil, der den Standort der Kühlung definiert, ist in einem der Kinder-Knoten eines <**CoolingBlocks**>-Knotens gegeben.
Der physikalische Teil ist der <**Cooling**>-Knoten darin.
Für beide Teile kann durch das Attribut "class" die jeweilige Art der Kühlung definiert werden, so dass beliebige Geometrien and Parametrierungen der Stärke der Kühlung miteinander kombiniert werden können.

<br/>

Geometrischer Teil
==========

<br/>

Prismatische Geometrie
----------

__Objekttyp: "CoolingPrismatic"__

<**Vertices**>: Eckpunkte des Polygons, das die Grundfläche des Prismas ist und das parallel zur xy-Ebene verläuft, in globalen kartesischen Koordinaten

<**LowerZCoordinate**>: z-Koordinate der Grundfläche des Prismas

<**UpperZCoordinate**>: z-Koordinate der Deckfläche des Prismas

<**Cooling**>: Physikalischer Teil (Parametrierung der Stärke) der Kühlung


Ein Prisma, dessen Grundfläche parallel zur xy-Ebene und deren Höhe parallel zur z-Achse verläuft.
\htmlinclude waterCooling_color.xml

Physikalischer Teil
==========

<br/>

Neumann-Randbedingung (konstante Wärmestromdichte [W/m²] durch die Kühlung)
----------

__Objekttyp: "CoolingByConstantValue"__

<**Value**>: Stärke der Kühlung in W/m²


\htmlinclude cooling_by_constant_color.xml


Look-Up-Tabelle: Wärmestromdichte abhängig von der Temperatur an der Grenzfläche Kühlung/thermischer Block.
----------

__Objekttyp: "CoolingByLookUp"__

<**LookUpTable**>: [Parameterobjekt](xmlobject.html) mit Definitionen der Messpunkte in W/m² für die Look-Up-Tabelle


Zwischen den Messpunkten der Look-Up-Tabelle wird linear interpoliert, wobei bei den Übergängen über einen Messpunkt leicht gerundet wird. Bei den Messpunkten handelt es sich um Temperaturen in °C.
\htmlinclude cooling_lookuptable_color.xml


Thermische Isolation
----------

__Objekttyp: "CoolingByLookUp"__

Kein Kontakt zur Umgebung, somit kein Wärmeübergang (adiabat).
\htmlinclude cooling_isolation_color.xml


Dirichlet-Randbedingung (vorgegebene Randtemperatur)
----------

__Objekttyp: "DirichletBoundaryCondition"__

Die Außenfläche, an der die Kühlung anliegt, wird konstant auf der vorgegebenen Temperatur gehalten.
\htmlinclude cooling_dirichlet_boundary_condition_color.xml

\~English

Consecutively all coolings, that are available in the thermal model, are listed.
For every cooling the structure of particular XML-file is described and an example for the code is given.
<br/>

General
=========
Every cooling has a geometrical (position in space) and a physical part (parameters of the cooling power).
The geometrical part, which describes the position of the cooling, is given in a child-node of a <**CoolingBlocks**>-node.
The physical part is the <**Cooling**>-node included.
For both parts it is possible to define the type of cooling by setting the attribute "class", so that any geometry and parameter sets of the cooling can be combined with each other.

<br/>

Geometrical part
==========

<br/>

Prismatic Geometry
----------

__Objecttype: "CoolingPrismatic"__

<**Vertices**>: Corner points of the polygon, which builds the the base of the prism and lies parallel to the xy-plane, in global cartesian coordinates.

<**LowerZCoordinate**>: z-coordinate of the base of the prism

<**UpperZCoordinate**>: z-coordinate of the top prism surface

<**Cooling**>:  Physical part (parametrization of the power) of the cooling


A prism, whose base lies parallel to the xy-plane and whose hight runs parallel to th z-axis.
\htmlinclude waterCooling_color.xml

Physical part
==========

<br/>

Neumann-boundary-value (constant heat flow density [W/m²] by the cooling)
----------

__Objecttype: "CoolingByConstantValue"__

<**Value**>: power of the cooling in W/m²


\htmlinclude cooling_by_constant_color.xml


Look-Up-table: heat flow density is dependant on the temperature of the boundary surface between cooling and thermal block.
----------

__Objecttype: "CoolingByLookUp"__

<**LookUpTable**>: [Parameterobject](xmlobject.html) with definitions of the measurement points in W/m² for the look-up-table


Between the measurements points of the look-up-table the curve is interpolated and is smoothed slightly at the measurement points. The measurement points are temperatures given in °C.
\htmlinclude cooling_lookuptable_color.xml


thermal isolation
----------

__Objecttype: "CoolingByLookUp"__

No contact to the environment, so no heat transmission (adiabatic).
\htmlinclude cooling_isolation_color.xml


Dirichlet-boundary-condition (given boundary temperature)
----------

__Objecttype: "DirichletBoundaryCondition"__

The outer surface, on wich the cooling is connected to, is held on a constant temperature.
\htmlinclude cooling_dirichlet_boundary_condition_color.xml
