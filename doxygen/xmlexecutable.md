Executable mit elektrisch/thermischer Simulation     {#xmlexecutable}
===============
\ifnot RELEASE_DOCU

\~German
Allgemein
=========

Die elektrisch/thermische Simulation wird mit variablem Stepsolver ausgeführt.
Es wird jeweils abwechselnd das elektrische und das thermische Modell simuliert.
Das elektrische Modell simuliert zuerst einen bestimmten Zeitraum, danach simuliert das thermische Modell genau diesen Zeitraum.
Wenn die Änderung des SoC oder der Temperatur in einem der Simulationsschritte zu groß ist, wird ein vorzeitiger Wechsel zwischen den Modellen erzwungen.

Ausführung
=========

Die Executable mit elektrisch/thermischer Simulation wird von der Konsole aus aufgerufen. Sie erhält vier Eingabeparameter:
- Die XML-Datei
- Ein Stromprofil
- Zeit, für die höchstens das elektrische oder thermische Modell ohne Wechsel läuft
- Gesamte Simulationszeit

Das Stromprofil besteht aus Angaben zum von außen gesetzten Strom zu einem bestimmten Zeitpunkt.
Das folgende Beispiel
- setzt den Strom bei 0 sec auf 5 A,
- bei 200 sec auf -2 A und
- ab 500 sec bis zum Ende der Simulation auf offene Klemmen (0 A).
~~~~~~~~~~~~~~~~~~~~~~
0, 5
200, -2
500, 0

~~~~~~~~~~~~~~~~~~~~~~
Falls nicht anders angegeben, wird der Strom bei 0 sec auf 0 A gesetzt.


Beispiel
=========

Als Beispiel für eine XML-Datei sei folgendes Simulationsszenario gegeben, das auch unter doxygen/examples/TestShowBattery.xml zu finden ist:
Diese könnte mit der Befehlszeile ausgeführt werden:

Executable TestShowBattery.xml Stromprofil.txt 10 1000

<pre>
\htmlinclude TestShowBattery_color.xml
</pre>
\endif


\~English
Abstract
=========

The electrical/thermal simulation is run with a variable step solver.
The electrical and thermal simulations are run alternately.
Firstly, the electrical model is simulating a certain period. Afterwards, the thermal model is simulating exactly the same period.
If the changes of the SoC or of the temperature are crossing the limits, a premature switch between the two models is forced.

Execution
=========

The executable with the electrical/thermal simulation is called via the terminal. This call contains four input parameters:
- The xml file
- A current profile
- Maximum runtime for the electrical or thermal model without a switch between both
- Entire simulation time

The current profile contains specifications of externally applied current at specific times.
The following example
- sets the current at 0s to 5 ampere (A),
- at 200 sec to -2 A
- from 500 sec until the end to open terminal(0 A).
~~~~~~~~~~~~~~~~~~~~~~
0, 5
200, -2
500, 0

~~~~~~~~~~~~~~~~~~~~~~
If not otherwise specified, the current is set to 0 A at 0 sec.


Example
=========

As an example for a xml file, following simulation scenario is given. *This scenario could also been found at doxygen/examples/TestShowBattery.xml*:
This could be run via terminal:

Executable TestShowBattery.xml Stromprofil.txt 10 1000

<pre>
\htmlinclude TestShowBattery_color.xml
</pre>
\endif
