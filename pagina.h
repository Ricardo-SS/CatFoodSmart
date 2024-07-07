const char* paginaHTML PROGMEM = R"=====(
//coloque aqui o codigo da pagina html, css e js.
<!DOCTYPE html>
<html lang="pt-br">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CAT FOOD</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f9f9f9;
        }

        img {
            max-width: 400px;
            border-radius: 5px 5px 0 0;
        }

        .container {
            display: flex;
            flex-direction: column;
            max-width: 400px;
            margin: 20px auto 0;
            background-color: #ffffff;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.1);
        }

        .logo {
            background-color: #017fdf;
            color: #ffffff;
            padding: 0.5rem;
            font-size: 36px;
            text-align: center;
            border-radius: 0 0 10px 10px;
        }

        .title {
            background-color: #017fdf;
            color: #ffffff;
            padding: 0.5rem;
            font-size: 20px;
            text-align: center;

        }

        .description {
            text-align: center;
            padding: 20px;
            font-style: italic;
            color: #017fdf;
        }

        .mensagem {
            font-size: 12px;
            text-align: center;
            font-style: italic;
            color: #017fdf;
        }

        .time-input {
            width: 100%;
            display: flex;
            gap: 0.8rem;
            justify-content: center;
            align-items: center;
        }

        .time {
            display: flex;

            flex-direction: column;
            width: 100%;
        
        }

        .time-input label {
            color: #000;
            font-weight: bold;
            margin-right: 10px;
            width: 100px;
        }

        .time-input input[type='number'] {
            align-items: center;
            width: auto;
            padding: 10px;
            /* margin-bottom: 10px; */
            border-radius: 5px;
            border: 1px solid #ccc;
        }

        input[type='submit'] {
            width: 100%;
            padding: 10px;
            margin-top: 20px;
            font-size: 1.1rem;
            background-color: #017fdf;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
    </style>
</head>

<body>
    <div class="logo">Cat Food</div>
    <div class="container">
        <img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQP47mgtAhNB50fyBmiYJsnCDy7yssPX4x06Q&usqp=CAU"
            alt="gato comendo">
        <div class="content-time">
            <div class="title">Alimentador Automático <span class="dog-icon">.</span></div>
            <div class="description">Cuidando bem do seu miu!</div>
            <form action="/horario" method="get">
                <div class="time-input">
                    
                    <div class="time">
                        <label for="hora">Hora:</label>
                        <input type="number" name="hora" placeholder="00" min="0" max="23" required>
                    </div>
                    <div class="time">
                        <label for="minuto">Minutos:</label>
                        <input type="number" name="minuto" placeholder="00" min="0" max="59" required>
                    </div>
                </div>
                <p class="mensagem">Preencha a hora de 00 a 23 e minutos de 0 a 59</p>
                <input type="submit" value="Salva">
            </form>
        </div>
    </div>
</body>

</html>

)=====";