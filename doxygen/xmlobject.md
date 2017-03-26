Parameterobjekte    {#xmlobject}
================
\~German

Parameterobjekte werden zur Parametrierung elektrischer Objekte verwendet.

Diese Objektgruppe enthält verschiedene Arten von Parameterobjekten:
+   Konstanter Parameterwert
+   Parameterwert nach gegebener Funktion berechnet
+   Nachschlagetabellen
+   Zustandsabhängige Nachschlagetabellen

<br/>

Allgemein
=========

Jedes XML-Element, das in der Konfiguration ein Parameterobjekt repräsentiert, besitzt das Attribut "class".
Über dieses Attribut wird der Objekttyp zugeordnet.
Parameterobjekte verwenden oft im Cache zwischengespeicherte Zustände für den Nachschlagevorgang.

<br/>

Parameterobjekte ohne Nachschlagetabelle
==========

<br/>

Konstantes Parameterobjekt
--------------------------

__Objekttyp: "ConstObj"__

<**Value**>: Wert

Definition eines konstanten Parameterobjektes:
\htmlinclude constobj_color.xml

<br/><br/>

Parameterobjekte mit eindimensionaler Nachschlagetabelle
========================================================

<br/>

1D-Parameterobjekt mit Zustandsbindung
----------

__Objekttyp: "LookupObj1dWithState"__ Attribut: LookupType = "LinearInterpolation" (default)

Für weitere Informationen zu LookupType siehe [LookupType](xmllookuptype.html)

<**State**>: [Zustandsobjekt](xmlstate.html). Muss per Objektreferenzierung übergeben werden (cacheref-Attribut).

<**LookupData**>: Kommaseparierte Messpunkte

<**MeasurementPoints**>: Kommaseparierte Messpunkte

Definition eines 1D-Parameterobjektes mit Zustandsbindung:
\htmlinclude 1dlookup_color.xml

<br/><br/>

Parameterobjekte mit zweidimensionaler Nachschlagetabelle
========================================================

<br/>

2D-Parameterobjekt mit Zustandsbindung
----------

__Objekttyp: "LookupObj2dWithState"__ Attribut: LookupType = "LinearInterpolation" (default)

Für weitere Informationen zu LookupType siehe [LookupType](xmllookuptype.html)

<**RowState**>: [Zustandsobjekt](xmlstate.html). Muss per Objektreferenzierung übergeben werden (cacheref-Attribut).

<**ColState**>: [Zustandsobjekt](xmlstate.html). Muss per Objektreferenzierung übergeben werden (cacheref-Attribut).

<**LookupData**>: Kommaseparierte Messpunkte in einer Tabellenzeile. Zeilen sind über Semikolon voneinander getrennt.

<**MeasurementPointsRow**>: Kommaseparierte Messpunkte der Zeilen

<**MeasurementPointsColumn**>: Kommaseparierte Messpunkte der Spalten


Definition eines 2D-Parameterobjektes mit Zustandsbindung:

\htmlinclude 2dlookup_color.xml

\~English

Parameter objects are used for the parameterization of electrical objects.

Those object groups contain several types of parameter objects:
+   Constant parameter values
+   Parameter values evaluated according to given functions
+   Lookup tables
+   Stateful lookup tables

<br/>

Abstract
=========

Each xml-element, which represents a parameter object in the configuration, possesses the attribute "class".
The object type is assigned via this attribute.
Parameter objects often use a cached state for the lookup process.

<br/>

Parameter objects without lookup tables
==========

<br/>

Constant parameter objects
--------------------------

__Object type: "ConstObj"__

<**Value**>: Value

Definition of a constant parameter object:
\htmlinclude constobj_color.xml

<br/><br/>

Parameter objects with one-dimensional lookup tables
====================================================

<br/>

1D parameter objects with state binding
----------

__Object type: "LookupObj1dWithState"__ attribute: LookupType = "LinearInterpolation" (default)

For further details on LookupType, see [LookupType](xmllookuptype.html)

<**State**>: [State object](xmlstate.html). Has to be passed via object reference (cacheref attribute).

<**LookupData**>: comma separated measurement points

<**MeasurementPoints**>: comma separated measurement points

Definition of a 1D parameter object with state binding:
\htmlinclude 1dlookup_color.xml

<br/><br/>

Parameter objects with two-dimensional lookup tables
========================================================

<br/>

2D parameter objects with state binding:
----------

__Object type: "LookupObj2dWithState"__ attribute: LookupType = "LinearInterpolation" (default)

For further details on LookupType, see [LookupType](xmllookuptype.html)(xmllookuptype.html)

<**RowState**>: [State object](xmlstate.html). Has to be passed via object reference(cacheref attribute).

<**ColState**>: [State object](xmlstate.html). Has to be passed via object reference(cacheref attribute).

<**LookupData**>: comma separated measurement points in a table row. Rows are separated via semicolon.

<**MeasurementPointsRow**>: comma separated measurement points of the rows

<**MeasurementPointsColumn**>: comma separated measurement points of the columns


Definition of a 2D parameter object with state binding:

\htmlinclude 2dlookup_color.xml