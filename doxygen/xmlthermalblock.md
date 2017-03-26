Thermische Blöcke     {#xmlthermalblock}
===================

\~German
Nachfolgend sind alle thermischen Blöcke, die im thermischen Modell verwendet werden können, aufgelistet.
Für jeden thermischen Block wird der Aufbau des dazugehörigen XML-Elementes beschrieben und ein Codebeispiel gegeben.

<br/>

Allgemein
=========

Jeder thermische Block kann mit dem Attribut "class" spezifiziert werden.
![Mögliche geometrische Blöcke](all_cells.png)
Jeder thermische Block kann das XML-Tag <**Heat**> haben.
Der darin angegebene Wert ist die Verlustleistung im Block in Watt.
Wenn die Verlustleistung an anderer Stelle der Simulation gesetzt wird (z.B. durch das elektrische Modell oder als Eingang der Simulink-S-Function), wird <**Heat**> ignoriert.

<br/>

Grundtypen
==========

<br/>

Quaderförmiger thermischer Block
----------
![Quaderförmiger thermischer Block](quadratic.png)

__Objekttyp: "RectangularBlock"__

<**Material**>: [Material](xmlthermalmaterial.html) des Quaders

<**Dimensions**>: Die drei Längen des Quaders in x-, y- und z-Richtung

<**SizeX**>: Diskretisierung des Quaders in finite Volumen in x-Richtung

<**SizeY**>: Diskretisierung des Quaders in finite Volumen in y-Richtung

<**SizeZ**>: Diskretisierung des Quaders in finite Volumen in z-Richtung

<**InitialTemperature**>: Anfangstemperatur im thermischen Block in °C

<**ElectricalDiscretization**>: Einteilung der elektrischen [Zellelemente](xmlelectrical.html) im thermischen Block. Diese muss für jede Dimension in die thermische Diskretisierung passen (z.B. für x-Dimension: SizeX modulo x == 0). Wenn <**ElectricalDiscretization**> nicht vorhanden ist, ist diesem Block nur ein Zellelement zugeordnet. Falls eines der Attribute x, y oder z fehlt, wird für dieses "1" angenommen.

\htmlinclude xmlrectangular_color.xml


Batteriezelle umgeben mit Füllmaterial in einem quaderförmigen Block
----------

__Objekttyp: "QuadraticCellBlock"__

<**Length**>: Länge der quadratischen Grundfläche

<**RadiusCell**>: Radius der Batteriezelle, die mittig im thermischen Block liegt

<**Height**>: Höhe (Länge in z-Richtung) des thermischen Blocks

<**CellRhoDivision**>: Diskretisierung der Batteriezelle in ρ-Richtung

<**PhiDivision**>: Diskretisierung des thermischen Blocks in φ-Richtung

<**HeightDivision**>: Diskretisierung des thermischen Blocks in z-Richtung

<**ArcPolygonization**>: Vorgeschlagene Strecke für die Polygonisierung der vorhandenen Bögen und Kreise

<**CellMaterial**>: [Material](xmlthermalmaterial.html) der Zelle

<**FillMaterial**>: [Material](xmlthermalmaterial.html) des Füllmaterials

<**InitialTemperature**>: Anfangstemperatur im thermischen Block in °C

<**ElectricalDiscretization**>: Einteilung der elektrischen [Zellelemente](xmlelectrical.html) im thermischen Block. Diese muss für jede dimension in die thermische Diskretisierung passen (z.B. für z-Dimension: HeightDivision modulo z == 0). Wenn <**ElectricalDiscretization**> nicht vorhanden ist, ist diesem Block nur ein Zellelement zugeordnet. Falls eines der Attribute phi oder z fehlt, wird für dieses "1" angenommen.

\htmlinclude xmlquadratic_color.xml


Supercapzelle mit Wickel aus zwei radialen Schichten umgeben mit Füllmaterial in einem quaderförmigen Block
----------

![Supercap mit zwei radialen Schichten](cell_cyl.png)

__Objekttyp: "Supercap"__

<**Length**>: Länge der quadratischen Grundfläche

<**InnerRadiusCell**>: Radius der Grenze zwischen innerer and äußerer Schicht des Wickels

<**OuterRadiusCell**>: Radius der Supercapzelle, die mittig im thermischen Block liegt

<**Height**>: Höhe (Länge in z-Richtung) des thermischen Blocks

<**PhiDivision**>: Diskretisierung des thermischen Blocks in φ-Richtung

<**HeightDivision**>: Diskretisierung des thermischen Blocks in z-Richtung

<**ArcPolygonization**>: Vorgeschlagene Strecke für die Polygonisierung der vorhandenen Bögen und Kreise

<**InnerCellMaterial**>: [Material](xmlthermalmaterial.html) der inneren Schicht der Supercapzelle

<**OuterCellMaterial**>: [Material](xmlthermalmaterial.html) der äußeren Schicht der Supercapzelle

<**FillMaterial**>: [Material](xmlthermalmaterial.html) des Füllmaterials

<**InitialTemperature**>: Anfangstemperatur im thermischen Block in °C

<**ElectricalDiscretization**>: Einteilung der elektrischen [Zellelemente](xmlelectrical.html) im thermischen Block. Diese muss für jede dimension in die thermische Diskretisierung passen (z.B. für z-Dimension: HeightDivision modulo z == 0). Wenn <**ElectricalDiscretization**> nicht vorhanden ist, ist diesem Block nur ein Zellelement zugeordnet. Falls eines der Attribute phi oder z fehlt, wird für dieses "1" angenommen.

\htmlinclude xmlsupercap_color.xml

Batteriezelle umgeben mit Füllmaterial in einem hexagonal prismatischen Block
----------

![Dreiecksprismaförmiger Block](cell_hex.png)

__Objekttyp: "HexagonalCellBlock"__

<**Length**>: Kantenlänge des gleichmäßigen Sechsecks, das die Grundfläche des thermischen Blocks bildet

<**RadiusCell**>: Radius der Batteriezelle, die mittig im thermischen Block liegt

<**Height**>: Höhe (Länge in z-Richtung) des thermischen Blocks

<**CellRhoDivision**>: Diskretisierung der Batteriezelle in ρ-Richtung

<**PhiDivision**>: Diskretisierung des thermischen Blocks in φ-Richtung

<**HeightDivision**>: Diskretisierung des thermischen Blocks in z-Richtung

<**ArcPolygonization**>: Vorgeschlagene Strecke für die Polygonisierung der vorhandenen Bögen und Kreise

<**CellMaterial**>: [Material](xmlthermalmaterial.html) der Zelle

<**FillMaterial**>: [Material](xmlthermalmaterial.html) des Füllmaterials

<**InitialTemperature**>: Anfangstemperatur im thermischen Block in °C

<**ElectricalDiscretization**>: Einteilung der elektrischen [Zellelemente](xmlelectrical.html) im thermischen Block. Diese muss für jede dimension in die thermische Diskretisierung passen (z.B. für z-Dimension: HeightDivision modulo z == 0). Wenn <**ElectricalDiscretization**> nicht vorhanden ist, ist diesem Block nur ein Zellelement zugeordnet. Falls eines der Attribute phi oder z fehlt, wird für dieses "1" angenommen.

\htmlinclude xmlhexagonal_color.xml


Dreiecksprismaförmiger thermischer Block
----------
![Dreiecksprismaförmiger Block](tri.png)

__Objekttyp: "TriangularPrismBlock"__

<**Vertices**>: Eckpunkte des Dreiecks, das die Grundfläche bildet, in xy-Koordinaten

<**Height**>: Höhe (Länge in z-Richtung) des Prismas

<**HeightDivision**>: Diskretisierung des Prismas in finite Volumen in z-Richtung

<**Material**>: [Material](xmlthermalmaterial.html) des Prismas

<**InitialTemperature**>: Anfangstemperatur im thermischen Block in °C

<**ElectricalDiscretization**>: Einteilung der elektrischen [Zellelemente](xmlelectrical.html) im thermischen Block in z-Richtung. Diese muss in die thermische Diskretisierung passen, also HeightDivision modulo z == 0. Wenn <**ElectricalDiscretization**> nicht vorhanden ist, ist diesem Block nur ein Zellelement zugeordnet. Falls das Attribut z fehlt, wird für dieses "1" angenommen.

\htmlinclude xmltri_color.xml

\~English
All thermal blocks, which are used in the thermal model, are listed in the section below.
The structure of the corresponding xml element and a code example is given for each block.

<br/>

Abstract
=========

Each thermal block can be specified by the attribute "class".
![Possible geometrical blocks](all_cells.png)
Each thermal block can posses the xml tag <**Heat**>.
The value indicated therein indicates the power loss in Watt.
If the power loss is set at another point in the simulation (e.g. in the electrical model or as input of the Simulink S-function), this value will be ignored.

<br/>

Base types
==========

<br/>

Rectangular thermal block
----------
![Rectangular thermal block](quadratic.png)

__Object type: "RectangularBlock"__

<**Material**>: [Material](xmlthermalmaterial.html) of the cuboid

<**Dimensions**>: The three dimensions of the cuboid in x-, y-, and z-direction

<**SizeX**>: Discretization of the cuboid in finite volumes in x-direction

<**SizeY**>: Discretization of the cuboid in finite volumes in y-direction

<**SizeZ**>: Discretization of the cuboid in finite volumes in z-direction

<**InitialTemperature**>: Initial temperature within the thermal block in °C

<**ElectricalDiscretization**>: Discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These has to fit into the thermal discretization in each dimension (e.g x-dimension: SizeX modulo x == o). If  <**ElectricalDiscretization**> is not existing, a single cell element will be addressed to this block. If one of the attributes x, y or z is missing, "1" will be used as default.

\htmlinclude xmlrectangular_color.xml


Battery cell surrounded by fill material within a rectangular block
----------

__Object type: "QuadraticCellBlock"__

<**Length**>: Length of the quadratic base area

<**RadiusCell**>: Radius of the battery cell, which is placed centrical within the thermal block.

<**Height**>: Height (length in z-direction) of the thermal block

<**CellRhoDivision**>: Discretization of the battery cell in ρ-direction

<**PhiDivision**>: Discretization of the thermal block in φ-direction

<**HeightDivision**>: Discretization of the thermal block in z-direction

<**ArcPolygonization**>:  Suggested route for the polygonization of the existing arcs and circles

<**CellMaterial**>: [Material](xmlthermalmaterial.html) of the cell

<**FillMaterial**>: [Material](xmlthermalmaterial.html) of the fill material

<**InitialTemperature**>: Initial temperature of the thermal block in °C

<**ElectricalDiscretization**>: Discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These has to fit into the thermal discretization in each dimension (e.g. z-dimension: HeightDivision modulo z == 0). If  <**ElectricalDiscretization**> is not existing, a single cell element will be addressed to this block. If one of the attributes phi or z is missing, "1" will be used as default.

\htmlinclude xmlquadratic_color.xml


Supercap cell with two radial layers surrounded by fill material within a rectangular block
----------

![Supercap with two radial layers](cell_cyl.png)

__Object type: "Supercap"__

<**Length**>: Length of the quadratic base area

<**InnerRadiusCell**>: Radius of the boundary between the inner and outer layer

<**OuterRadiusCell**>: Radius of the supercap cell, which is placed centrical within the thermal block

<**Height**>: Height (length in z-direction) of the thermal block

<**PhiDivision**>: Discretization of the thermal block in φ-direction

<**HeightDivision**>: Discretization of the thermal block in z-direction

<**ArcPolygonization**>: Suggested route for the polygonization of the existing arcs and circles

<**InnerCellMaterial**>: [Material](xmlthermalmaterial.html) of the inner layer of the supercap cell

<**OuterCellMaterial**>: [Material](xmlthermalmaterial.html) of the outer layer of the supercap cell

<**FillMaterial**>: [Material](xmlthermalmaterial.html) of the fill material

<**InitialTemperature**>: Initial temperature of the thermal block in °C

<**ElectricalDiscretization**>: Discretization of the electrical [cell elements](xmlelectrical.html) within the thermal block. These has to fit into the thermal discretization in each dimension (e.g. z-dimension: HeightDivision modulo z == 0). These has to fit into the thermal discretization in each dimension (e.g. z-dimension: HeightDivision modulo z == 0). If  <**ElectricalDiscretization**> is not existing, a single cell element will be addressed to this block. If one of the attributes phi or z is missing, "1" will be used as default.

\htmlinclude xmlsupercap_color.xml

Battery cell surrounded  with fill material in an hexagonal prismatic block
----------

![Hexagonal prismatic block](cell_hex.png)

__Object type: "HexagonalCellBlock"__

<**Length**>: Edge length of the regular hexagon, which provides the base area of the block

<**RadiusCell**>: Radius of the cell, which is placed centrical within the thermal block

<**Height**>: Height (length in z-direction) of the thermal block

<**CellRhoDivision**>: Discretization of the battery cell in ρ-direction

<**PhiDivision**>: Discretization of the thermal block in φ-direction

<**HeightDivision**>: Discretization of the thermal block in z-direction

<**ArcPolygonization**>: Suggested route for the polygonization of the existing arcs and circles

<**CellMaterial**>: [Material](xmlthermalmaterial.html) of the cell

<**FillMaterial**>: [Material](xmlthermalmaterial.html) of the fill material

<**InitialTemperature**>: Initial temperature of the thermal block°C

<**ElectricalDiscretization**>: Discretization of the electric [cell elements](xmlelectrical.html) within the thermal block. These has to fit into the thermal discretization in each dimension (e.g. z-dimension: HeightDivision modulo z == 0).If  <**ElectricalDiscretization**> is not existing, a single cell element will be addressed to this block.  If one of the attributes phi or z is missing, "1" will be used as default.

\htmlinclude xmlhexagonal_color.xml


Triangular prismatic thermal block
----------
![Triangular prismatic thermal block](tri.png)

__Object type: "TriangularPrismBlock"__

<**Vertices**>: Vertices of the triangle, which forms the base are; in xy-coordinates

<**Height**>: Height (length in z-direction) of the prism

<**HeightDivision**>: Discretization of the prism in finite volumes in z-direction

<**Material**>: [Material](xmlthermalmaterial.html) of the prism

<**InitialTemperature**>: Initial temperature of the thermal block in °C

<**ElectricalDiscretization**>: Discretization of the electric [cell elements](xmlelectrical.html) within the thermal block in z-direction. This has to fit into the thermal discretization (HeightDivision modulo z == 0) . If <**ElectricalDiscretization**> is not existing a single cell element will be addressed to this block. It the attribute z is missing, "1" will be used as default.

\htmlinclude xmltri_color.xml

