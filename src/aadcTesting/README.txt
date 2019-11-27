ReadMe für ADTF Simulationsumgebung (GUI Matlab/Simulink)

Das GUI „Testumgebung_MatlabSimulink“ dient als Grundlage, um ADTF-Filter in Matlab/Simulink verwenden zu können. Aktuell ist das GUI auf das Testen ausgelegt, es kann aber auch als Schnittstelle zwischen dem C++ Code und z.B. Carmaker oder anderen Simulationstools verwendet werden. 
Das GUI dient zum Plotten von In- & Outputs des ADTF-Filters, um eine schnelle Aussage über die Funktionsfähigkeit des ADTF-Filters treffen zu können.
Entwickelt wurde das GUI, um die Funktion eines PID-Reglers zu testen und die Auslegung zu vereinfachen. Es ist aber auch möglich, andere ADTF-Filter zu verwenden. 

1.	Input
	a. Anzahl Inputs
	Hier muss die Anzahl an Inputs des ADTF-Filters angegeben werden.

	b. .csv - Datei laden
	Die Spalten der Datei müssen mit der Anzahl an Inputs übereinstimmen.
	Die Anzahl der Zeilen kann beliebig groß sein, muss allerdings in jeder Spalte gleich lang sein. 
	Die erste Zeile der Datei ist für eine Headerzeile reserviert und wird nicht geladen.

2.	Output
	a. Anzahl Outputs
	Hier muss die Anzahl an Outputs des ADTF-Filters angegeben werden.

3.	C++ Code
	a.	Umwandlung des C++Codes in MATLAB S-function
	Aktuell ist es noch nicht möglich einen ganzen ADTF-Filter in eine für MATLAB kompatible S-function source file umzuwandeln. Um diesen trotzdem in MATLAB/Simulink nutzen zu können, muss der bestehende 
	C++Code und die korrespondierende Header-Datei minimal angepasst werden. Dadurch, dass das Problem lediglich durch bestimmte ADTF-Abhängigkeiten und Funktionen, die auf der ADTF-Lib basieren, 
	zurückzuführen ist, müssen diese abgeändert werden (C++- und C-Libs werden unterstützt). Sind all diese ADTF-Abhängigkeiten ersetzt worden, wird schließlich mit der MATLAB-function ‚cpp2mat()‘ die S-	function gebildet.

	b.	C++ Code laden
	Zu testender C++ Code muss aus einem Source- & einem Headerfile bestehen.
	
		i.	Auswahl C++ Files
			1. Auswahl des Headerfiles
			2. Auswahl des Sourcefiles

		ii.	Simulink Modell aus Sourcecode erstellen 
		Im Aktuellen Ordner ist bereits die slx-Datei (Simulink-Modell) ‚DasBlaueAuto_Controller_Model‘. Die bereits erwähnte cpp2mat-function erstellt einen Simulink-S-function-Block mit der in 3a. erstellten S-function source file als Quelle in das erwähnte Simulink-Modell. 
		Wird ein neues Simulink-Modell erstellt, in das der S-function-Block samt S-function erstellt werden soll, muss im cpp2mat-Skript im Bereich ‚Assign Simulink-Model to 'sys'‘ der Name hinter der Variablen ‚sys‘ ausgetauscht werden.

		Falls ein gleichnamiger S-function-Block bereits im Simulink-Modell existiert, wird die Warnung ‚Error! Check if required Simulink-Block is already existing!‘ ausgegeben.


		iii. Automatische Generierung von In-/Outputs in Simulink um mit dem Workspace zu arbeiten
		Mit der Funktion „configureSimulinkModel“, werden an dem generierten Simulink-Modell automatisch In-/Outputs hinzugefügt, um das Simulink Modell auch in Matlab nutzen zu können, die Inputs aus der csv-Datei zu verwenden und die Ausgabe in Abschnitt 4 realisieren zu können

	c.	C++ Code eingeben (noch nicht umgesetzt)


4.	Ausgabe
	a.	Plot Einschwingverhalten
	Mit Aktivierung dieser Checkbox wird das Ein- /Ausgangsverhalten des Filters im Plot dargestellt.

	b.	Bodeplot (noch nicht umgesetzt)

	c.	Simulationszeit
	Die Simulationszeit verändert die Simulationsdauer in Simulink. 
	Indirekt wird hierüber festgelegt, wie lange die Werte aus einer Zeile der Input-csv in der Simulation verwendet wird. Jede Zeile aus der Input-csv wird gleich lange in der Simulation verwendet.
	Bsp.: Die Daten in der Input-csv umfassen 3 Zeilen, die Simulationszeit beträgt 10s und der Simulationsschritt beträgt immer 0.2s -> 10s/3 = 3s (das bedeutet, jeder Wert wird 3s lang als Input verwendet – in der letzten Sekunde wird der Wert der letzten Zeile weiterhin ausgeführt).

	d.	Simulieren
	Hier wird das zuvor erstellte Simulinkmodell mit den automatisch hinzugefügten In-& Outputs ausgeführt. Simulationsergebnisse werden geplottet, wenn „Plot Einschwingverhalten“ aktiviert ist.

	e.	Report erstellen (noch nicht umgesetzt)
