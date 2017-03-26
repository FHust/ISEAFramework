Elektrische Objekte     {#xmlelectrical}
===================

Nachfolgend sind alle elektrischen Objekttypen, aus denen ein Netzwerk aufgebaut werden kann, aufgelistet.
Für jeden Objekttyp wird der Aufbau des dazugehörigen XML-Elementes beschrieben und ein Codebeispiel gegeben.

<br/>

Allgemein
=========

Jedes XML-Element, das in der Konfiguration ein elektrisches Objekt repräsentiert, besitzt das Attribut "class".
Über dieses Attribut wird der Objekttyp zugeordnet.
Außerdem kann jedes XML-Element das Attribut "observable='true'" besitzen.
Durch dieses Attribut werden Strom, Spannung, Verlustleistung und ggf. Soc des Objektes berechnet und bei Verwendung des Datenloggers mit ausgegeben.

<br/>

Grundtypen
==========

<br/>

Widerstand
----------

__Objekttyp: "OhmicResistance"__

<**Object**>: [Parameterobjekt](xmlobject.html) mit Widerstandswert

Definition eines Widerstandes mit konstantem Widerstandswert:
\htmlinclude ohmic_color.xml


Kondensator
----------

__Objekttyp: "Capacity"__

<**Object**>: [Parameterobjekt](xmlobject.html) mit Kapazitätswert

Definition eines Kondensators mit konstantem Kapazitätswert:
\htmlinclude capacity_color.xml


Spannungsquelle
----------

__Objekttyp: "VoltageSource"__

<**Object**>: [Parameterobjekt](xmlobject.html) mit Spannungswert

Definition einer Spannungsquelle mit konstantem Spannungswert:
\htmlinclude voltage_color.xml


Parallelschaltung
----------

__Objekttyp: "ParallelTwoPort"__

<**Children**>: Enthält weitere XML-Elemente. Eines für jeden Zweig der Parallelschaltung.

Definition einer Parallelschaltung aus einem Kondensator und einem Widerstand:
\htmlinclude parallel_color.xml


Serienschaltung
----------

__Objekttyp: "SerialTwoPort"__

<**Children**>: Enthält weitere XML-Elemente, die in Serie verschaltet sind.

Definition einer Serienschaltung aus einem Kondensator und einem Widerstand:
\htmlinclude serial_color.xml

<br/><br/>

Komplexere Bauelemente
======================

<br/>

Zarc-Element
----------

__Objekttyp: "ZarcElement"__

<**LookupTau**>: [Parameterobjekt](xmlobject.html) für Tau. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**LookupOhmicResistance**>: [Parameterobjekt](xmlobject.html) für Widerstand. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**LookupPhi**>: [Parameterobjekt](xmlobject.html) für Phi. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

Bei der Verwendung von Zarc-Elementen sollte der Benutzer die minimale Samplerate als [Option](xmloption.html) <**SampleRate**> festlegen:
\htmlinclude examples/samplerate_color.xml

Definition eines Zarc-Elementes:
\htmlinclude examples/zarc_color.xml

Diffusion - Cotanh
----------

__Objekttyp: "WarburgCotanh"__
Attribut "WithCapacity":  "True" (default)
Wenn das Attribut **WithCapacity** auf false gesetzt wird, dann wird keine zusätzliche Kapazität zum Ersatzschaltbild hinzugefügt und diese kann über eine Spannungsquelle modelliert werden.

Attribut "RCCounter":  "5" (default)
Über das Attribut **RCCounter** kann die Genauigkeit der Approximation des Diffusionelementes eingestellt werden. Je höher dieser Wert ist, desto genau die Approximation. Die SampleRate limitiert hierbei auch die Anzahl der RC-Glieder.

<**Sigma**>: [Parameterobjekt](xmlobject.html) für Sigma. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**Clim**>: [Parameterobjekt](xmlobject.html) für limiting Capacity. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".


Bei der Verwendung von Cotanh-Elementen sollte der Benutzer die minimale Samplerate als [Option](xmloption.html) <**SampleRate**> festlegen:
\htmlinclude examples/samplerate_color.xml

Definition eines Warburg-Cotanh:
\htmlinclude examples/warburg_coth_color.xml

Diffusion - Tanh
----------

__Objekttyp: "WarburgTanh"__

Attribut "RCCounter":  "5" (default)
Über das Attribut **RCCounter** kann die Genauigkeit der Approximation des Diffusionelementes eingestellt werden. Je höher dieser Wert ist, desto genauer die Approximation. Die SampleRate limitiert hierbei auch die Anzahl der RC-Glieder.

<**OhmicResistance**>: [Parameterobjekt](xmlobject.html) für den Ohmschen Anteil. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".

<**Tau**>: [Parameterobjekt](xmlobject.html) für die Relaxationskonstante. Dieses Parameterobjekt verwendet ty­pi­scher­wei­se die zwei [Zustände](xmlstate.html) "ThermalState" und "Soc" des übergeordneten "CellElements".


Bei der Verwendung von Tanh-Elementen sollte der Benutzer die minimale Samplerate als [Option](xmloption.html) <**SampleRate**> festlegen:
\htmlinclude examples/samplerate_color.xml

Definition eines Warburg-Tanh:
\htmlinclude examples/warburg_tanh_color.xml




Batterie
----------

__Objekttyp: "CellElement"__

<**ThermalState**>: [Zustandsobjekt](xmlstate.html) vom Typ "ThermalState". Dieses Objekt sollte im Cache zwischengespeichert werden, damit es für die Nachschlagetabellen weiterverwendet werden kann.

<**Soc**>: [Zustandsobjekt](xmlstate.html) vom Typ "Soc". Dieses Objekt sollte im Cache zwischengespeichert werden, damit es für die Nachschlagetabellen weiterverwendet werden kann.

<**Children**>: Enthält weitere XML-Elemente, die in Serie verschaltet sind. Eine Batterie kann aus beliebigen elektrischen Bauteilen aufgebaut sein.


Definition einer Beispielbatterie:
\htmlinclude examples/cellelement_color.xml
