LookupType    {#xmllookuptype}
================
\~German

[Parameterobjekte] (xmlobject.html) mit ein- oder zweidimensionalen Nachschlagetabellen  besitzen das Attribut LookupType. Für dieses sind die folgenden Parameter möglich:

- "LinearInterpolation" (Lineare Interpolation der Stützwerte zwischen je zwei Stützstellen.) Wird das Attribut LookupType nicht gesetzt, wird per default "LinearInterpolation" angenommen.
- "NearestNeighbour" (Wählt den Stützwert, der der aktuellen Stützstelle am nächsten ist.)
- "ConstantUntilThreshold" (Behält den aktuellen Stützwert bei, bis die zugehörige Stützstelle überschritten wird).
- "SplineInterpolation" (Kubische Interpolation der Stützwerte zwischen je zwei Stützstellen. Der Übergang der Stützwerte an den Stützstellen ist glatt, d.h. die erste und die zweite Ableitung der Interpolationsfunktion existieren an diesen Stellen.)

\~English

[Parameter objects] (xmlobject.html) with one or two-dimensional lookup tables possess the attribute LookupType. For these the following parameters are possible:

- "LinearInterpolation" (linear interpolation of the supporting values between two grid points) The default parameter "LinearInterpolation" will be used, if no Lookuptype attribute is set.
- "NearestNeighbour" (chooses the next possible supporting value to the recent grid point)
- "ConstantUntilThreshold" (maintains the current supporting value as long as the related grid point is not crossed).
- "SplineInterpolation" (cubic interpolation  of the supporting values between two grid points. The transition of the supporting values at the grid points is smooth, i.e the first and second derivative of the function exist.
.)
