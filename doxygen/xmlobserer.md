Observer      {#xmlobserver}
=======================
Nachfolgend sind alle Observerfilter aufgeführt.

<br/>

Allgemein
=========
Die Ausgabe der elektrischen Simulation läßt sich ausser im Falle von Simulink per XML-Datei beeinflussen, über sogenannte Filter.
Die Filter werden dabei seriell abgearbeitet vom ersten Filter bis zum letzten.

![Filterkette für die Ausgabe des elektrischen Modells](filterChain.png)
<br/>

Dazu muss in der XML-Datei der unter dem Knoten <Configuration> das Element <Observer> erstellt werden.
Innerhalb des <Observer>-Tags können eine beliebige Anzahl an Filtern hinzugefügt.



<br/>
CSV-Filter
==========

Der CSV-Filter generiert eine CSV-Datei mit den Werten im folgende Format:

<pre>
#Time; Elementnr; Voltage; Current; Power; SOC
#s; Number ; Voltage / V; Current / A; Power / W; SOC / %
</pre>

\arg Time:  aktuelle Simulationzeit in Sekunden
\arg Elementnr: Nummer des  elektrisches Element für den die folgenden Werte gelten
\arg Voltage: Spannung in Volt
\arg Current: Strom in Ampere
\arg Power: thermische Leistung in Watt
\arg SOC: Ladezustand (state of charge) in %

\htmlinclude csvfilter_color.xml

<br/>
STDOut-Filter
==========
Der STDOut-Filter gibt die Werte in ähnlicher Art und Weise wie der CSV-Filter aus.
Jedoch werden die Daten nicht in einer Datei gespeichert sondern direkt über die Standarausgabe ausgegeben.

\htmlinclude stdoutfilter_color.xml
<br/>

Dezimierungs-Filter
========
Der Dezimierungsfilter verhindert, dass zu viele Daten ausgegeben werden. Es werden nur Daten mit einem Simulationszeitstempel, die einen zeitlichen  Mindestabstand einhalten durchgelassen (dezimiert).
Dies ist sinnvoll wenn man mit sehr vielen Daten Rechnen muss, ohne dass diese Daten viel mehr Information bringen.


\htmlinclude decimatefilter_color.xml

<br/>
Matlab-Filter
========
Die Daten werden im Matlab Format (.mat) abgespeichert. Die Daten werden dabei gezippt, sind damit sehr viel platzsparender als im CSV-Format und können direkt über Matlab eingelesen werden.


\htmlinclude matlabFilter_color.xml

<br/>
Beispiel: Verkettung von Filtern
======
Im folgenden wird eine Filterkette aus einer Matlab Ausgabe, einer Dezimierung,  Stdout-Filter sowie einem CSV-Filter gebildet.<br/>
Dabei werden alle Ausgaben des Modells in der Matlab zwischengespeichert. <br/>
Danach wird eine Dezimierung der Daten durchgeführt, die einen zeitlichen Mindestetabstand von 0.1 Sekunden der Datenpunkte garantiert.<br/>
Diese Daten werden dann auf die Standardausgabe ausgeben und in eine CSV-Datei gespeichert<br/>

\htmlinclude filterChain_color.xml
