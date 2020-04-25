const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>Follower Counter</TITLE>
            <style>

                html {
                font-size: 62.5%;
                box-sizing: border-box;
                }

                *, *::before, *::after {
                margin: 0;
                padding: 0;
                box-sizing: inherit;
                font-family: sans-serif;
                }

                
                a { 
                    display: inline-block; 
                    padding: 4px 7px 3px 7px; 
                    margin: 2px; 
                    background: #2166de; 
                    color: #fff; 
                    text-decoration: none;
                    line-height: 30px;
                }
                
                h1 { font-size: 24px; margin: 5px;}
                h3 { font-size: 18px; border-bottom: 1px solid #ddd; margin: 5px; }
                .buttongrid  {
                     display: grid;
                    grid-template-columns: 1fr 1fr 1fr 1fr;
                }

                .buttongrid a {
                    display: block;
                    text-align: center;
                    line-height: 30px;
                }

            
            </style>
            <meta name = "viewport" content = "width = device-width">
            <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>

	</HEAD>
<BODY>

    <h1>Follower Counter</h1>
	
    <h3>Funktionen</h3>
    <a href="/reset">Neustart</a>
    <a href="/update">Update</a>
    <a href="/format">Zurücksetzen</a>


    <a href="/mode1">Nur Counter</a>
    <a href="/mode2">Nur Uhr</a>
    <a href="/mode3">Uhr und Counter im Wechsel</a>

    <h3>Helligkeit</h3>

    <div class="buttongrid">

        <a href="/intensity?intensity=0">0</a> 
        <a href="/intensity?intensity=1">1</a> 
        <a href="/intensity?intensity=2">2</a> 
        <a href="/intensity?intensity=3">3</a> 
        <a href="/intensity?intensity=4">4</a> 
        <a href="/intensity?intensity=5">5</a> 
        <a href="/intensity?intensity=6">6</a> 
        <a href="/intensity?intensity=7">7</a>
        <a href="/intensity?intensity=8">8</a> 
        <a href="/intensity?intensity=9">9</a> 
        <a href="/intensity?intensity=10">10</a> 
        <a href="/intensity?intensity=11">11</a> 
        <a href="/intensity?intensity=12">12</a> 
        <a href="/intensity?intensity=13">13</a> 
        <a href="/intensity?intensity=14">14</a> 
        <a href="/intensity?intensity=15">15</a> 
    
    </div>
</BODY>
</HTML>
)=====";