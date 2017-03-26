Thermische Materialien     {#xmlthermalmaterial}
===================

\~German
Nachfolgend sind alle Materialien, die im thermischen Modell verwendet werden können, aufgelistet.
Für jedes Material wird der Aufbau des dazugehörigen XML-Elementes beschrieben und ein Codebeispiel gegeben.

<br/>

Allgemein
=========

Alle Materialien sind im <**ThermalMaterials**>-Knoten, der im Wurzelknoten der XML-Datei (im <**Configurations**>-Knoten) zu finden ist, aufgelistet.
Alle Materialien müssen das Attribut "cache='true'" gesetzt haben.

<br/>

Thermische Materialien
==========

<br/>

Material mit isotroper Wärmeleitfähgkeit
----------

__Objekttyp: "Material"__

<**Density**>: Dichte in kg/m³

<**SpecificCapacity**>: Spezifische Wärmekapazität in J/(kg·K)

<**Conductivity**>: Wärmeleitfähigkeit in W/(m·K)

\htmlinclude thermalmaterial_color.xml

<br/>


Material mit anisotroper Wärmeleitfähigkeit gegeben in kartesischen Koordinaten
----------

__Objekttyp: "MaterialCartesian"__

<**Density**>: Dichte in kg/m³

<**SpecificCapacity**>: Spezifische Wärmekapazität in J/(kg·K)

<**ConductivityX**>: Wärmeleitfähigkeit in x-Richtung in W/(m·K)

<**ConductivityY**>: Wärmeleitfähigkeit in y-Richtung in W/(m·K)

<**ConductivityZ**>: Wärmeleitfähigkeit in z-Richtung in W/(m·K)


\htmlinclude thermalmaterialcartesian_color.xml

<br/>


Material mit anisotroper Wärmeleitfähigkeit gegeben in zylindirschen Koordinaten
----------

__Objekttyp: "MaterialCylindric"__

<**Density**>: Dichte in kg/m³

<**SpecificCapacity**>: Spezifische Wärmekapazität in J/(kg·K)

<**ConductivityRho**>: Wärmeleitfähigkeit in ρ-Richtung in W/(m·K)

<**ConductivityPhi**>: Wärmeleitfähigkeit in φ-Richtung in W/(m·K)

<**ConductivityZ**>: Wärmeleitfähigkeit in z-Richtung in W/(m·K)

\htmlinclude thermalmaterialcylindric_color.xml

\~English
All materials, which are used in the thermal model, are listed below.
The structure of the corresponding xml elements and a code snippet are given for each material.

<br/>

Abstract
=========

All material are listed in the <**ThermalMaterials**>-node, which could be found in the root node of the xml file (within the <**Configurations**>-node).
The attribute "cache='true'" has to be set for all materials.

<br/>

Thermal materials
==========

<br/>

Material with isotropic thermal conductivity
----------

__Object type: "Material"__

<**Density**>: density in kg/m³

<**SpecificCapacity**>: specific heat capacity in J/(kg·K)

<**Conductivity**>: thermal conductivity in W/(m·K)

\htmlinclude thermalmaterial_color.xml

<br/>


Material with anisotropic thermal conductivity in Cartesian coordinates
----------

__Object type: "MaterialCartesian"__

<**Density**>: density in kg/m³

<**SpecificCapacity**>: specific heat capacity in J/(kg·K)

<**ConductivityX**>: thermal conductivity in x-direction in W/(m·K)

<**ConductivityY**>: thermal conductivity in y-direction in W/(m·K)

<**ConductivityZ**>: thermal conductivity in z-direction in W/(m·K)


\htmlinclude thermalmaterialcartesian_color.xml

<br/>


Material with anisotropic thermal conductivity in cylindrical coordinates
----------

__Objekttyp: "MaterialCylindric"__

<**Density**>: density in kg/m³

<**SpecificCapacity**>: specific heat capacity in J/(kg·K)

<**ConductivityRho**>: thermal conductivity in ρ-direction in W/(m·K)

<**ConductivityPhi**>: thermal conductivity in φ-direction in W/(m·K)

<**ConductivityZ**>: thermal conductivity in z-direction in W/(m·K)

\htmlinclude thermalmaterialcylindric_color.xml

