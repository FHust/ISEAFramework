Solversystem     {#system}
============
\ifnot RELEASE_DOCU

\~German

Das Solversystem berechnet ein elektrisches Netzwerk ausgehend von den Maschen- und Differentialgleichungen.<br/>
Dafür wird der Zustandvektor des Zustandsraummodells für einen Simulationszyklus neuberechnet.

Über den Konstruktor werden der Klasse die systm::StateSystemGroup und der Wert für dT übergeben.<br/>
Dabei ist dT die Dauer eines einzelnen Simulationschrittes.

__Wichtig:__ Für dT gleich Null wird ein Solver mit __variabler Schrittweite__ eingesetzt.

Der Konstruktor der Klasse systm::System:

~~~~~~~~~~~~~
System(StateSystemGroup< T >* stateSystemGroup, double dt = 0);
~~~~~~~~~~~~~


Hier ein Codebeispiel für die Verwendung der Klasse systm::System:

\htmlinclude examples/solverbsp_color.xml
\endif


\~English

The equation solver computes an electrical network based on mash equations and linear differential equations.<br/>
In order to do this, the state vector of the state space model is recalculated for the next simulation iteration.

Via the constructor the class gets systm::StateSystemGroup and the value of dT.<br/>
dT is the size of a single simulation step.

__Important:__ If dT equals zero, a solver with __variable step size__ is used.

The constructor of the class systm::System:

~~~~~~~~~~~~~
System(StateSystemGroup< T >* stateSystemGroup, double dt = 0);
~~~~~~~~~~~~~

This is an example for a code for the usage of the class systm::System:

\htmlinclude examples/solverbsp_color.xml
\endif
