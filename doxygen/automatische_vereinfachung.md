Automatisierte Vereinfachung      {#xmlvereinfachung}
==========

\~German
Die große Bandbreite an Zeitkonstanten in einem Modell kann häufig zu Problemen führen.
Insbesondere können sehr schnell große Fehler durch kleine Zeitkonstanten entstehen.

Deswegen wurde die Option \<SampleRate\> eingeführt.
Anhand dieser Option kann eine automatisierte Reduktion der Zeitkonstanten erfolgen.
Falls eine Zeitkonstante \f$\ll\f$ als das Sampling-Intervall ist, wird diese Zeitkonstante vereinfacht.
Die Vereinfachung wird durchgeführt, wenn gilt: 3*tau < 1/SampleRate.
Im Bild unten ist beispielsweise die Reduktion eines Modells für eine Samplerate von einem 1 Hz dargestellt.

![Reduktion der Komplexität](reduction_of_complexity.png)

ACHTUNG
---
Eine Reduktion ist für das Echtzeitboard notwendig.
Falls die \<SampleRate\> und die genutzte Diskretisierungsrate des Modells nicht passend gewählt sind, kann es zu Linearisierungsfehlern und damit zu einem instabilen System kommen.
Die Eulerintegration kann nur Zeitkonstanten erfassen für die gilt, dass alle in der Simulation enthaltenen \f$ tau > 1/2 * dt \f$ sind, wobei dt die feste Zeitschrittweite kennzeichnet.
Nach der Vereinfachung gilt für alle im Modell verbleibenden Zeitkonstanten:

\f[ tau > 1/(3*SampleRate) \stackrel{!}{>} 1/2 dt \f]

Hierdurch ergibt sich ein Zusammenhang zwischen SampleRate und Zeitschrittweite.
Wird einer von den beiden Parametern festgesetzt, kann daraus eine Grenze für den anderen festgelegt werden, damit das System stabil läuft.
Kleine dt erlauben es kleinere Zeitkonstanten abzubilden, somit kann die Samplerate größer gewählt werden. Allerdings sollte die Wahl der Parameter großzügig ausfallen,
 da das System sonst zwar konvergiert, aber zum Überschwingen neigt. In der unteren Abbildung ist die Integration mit dem Eulerverfahren für zwei RC-Glieder mit unterschiedlichen Zeitkonstanten gezeigt.

 ![Eulerverfahren mit verschiedenen Zeitkonstanten](EulerStabilitaet2.png)

\~English
The wide band of time constants, which can be present in the model, might lead to troubles with stability.
In the case of very small time constants a great error might occur.

Therefore the option \<SampleRate\> has been added.
With this option an automatized reduction of the system can be achieved.
The time constant can be neglected if the time constant is \f$\ll\f$  than the sampling interval of the solver.
The model reduction can be used if: 3*tau < 1/sample rate.
A reduction of the model with the sample rate of 1 Hz is depicted in the figure below.


![Reduction of complexity](reduction_of_complexity.png)

ATTENTION
---
A model reduction for the real-time board is required.
In case of unmatched \<SampleRate\> and used discretisation of the model a linearization error will occur.
This might lead to an instable model.
Euler integration (ODE1) can only converge if all time constants obey the rule \f$ tau > 1/2 * dt \f$ , where dt is the constant time step of the solver.
Afterwards, the following time constants remain:

\f[ tau > 1/(3*SampleRate) \stackrel{!}{>} 1/2 dt \f]


This equation describes the relationship between SampleRate and time step.
In case that one parameter is fixed, the other can be determined for a stable system.
A small dt allows to simulate smaller time constants and therefore the sample rate has to be increased.
The increase should be considered with a high tolerance, as the convergences is impacted drastically.
If the tolerance is too low, oscillation might occur.
In the figure below, the simulations of two RC-elements with different time constants are shown.
A constant current is applied and the system is solved by euler integration.
It can be seen that one numerical integration does not converge while the other with a higher tolerance converges.


 ![Euler integration with different time constants](EulerStabilitaet2.png)

