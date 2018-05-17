defmodule Router do

  @my_router :my_awesome_router

  # API
  def start() do
    pid = spawn(Router, :route_messages, [%{}, %{}, %{}])
    :global.register_name @my_router, pid
  end

  def stop() do
    :global.send @my_router, :shutdown
  end

  def send_chat_message(message_body, addressee) do
    :global.send @my_router, {:send_chat_message, addressee, message_body}
  end

  def broadcast_message(message_body) do
    :global.send @my_router, {:broadcast_message, message_body}
  end

  def challenge_player(nickname, other_nickname) do
    :global.send @my_router, {:challenge_player, nickname, other_nickname}
  end

  def challenge_answer(player_who_challenged, choice) do
    :global.send @my_router, {:challenge_answer, player_who_challenged, choice}
  end

  def broadcast_status(nickname, other_nickname, status) do
    :global.send @my_router, {:broadcast_status, nickname, other_nickname, status}
  end

  def interpret_choice(nickname, choice)do
    :global.send @my_router, {:interpret_choice, nickname, choice}
  end

  def broadcast_positions(nickname_of_receiver) do
    :global.send @my_router, {:broadcast_positions, nickname_of_receiver}
  end

  def update_positions(nickname, new_x_position, new_y_position)do
    :global.send @my_router, {:update_positions, nickname, new_x_position, new_y_position}
  end

  def get_player_position(nickname, client_socket) do
    :global.send @my_router, {:get_player_position, nickname, client_socket}
  end

  def register(nickname, client_socket, x, y) do
    :global.send @my_router, {:register, nickname, client_socket, x , y}
  end

  def unregister(nickname) do
    :global.send @my_router, {:unregister, nickname}
  end

  def route_messages(clients, clients_positions, game_choices) do
    receive do
      # Talk to different machines
      # Robustness
      # Distributing to clients directly
      {:send_chat_message, addressee, message_body} ->
        case Map.get(clients, addressee) do
        nil ->
          IO.puts "Addressee #{addressee} unknown."
        client_socket ->
            :gen_tcp.send client_socket, message_body
        end
        route_messages(clients, clients_positions, game_choices)

      {:get_player_position, nickname, client_socket}->
        case Map.get(clients_positions, nickname) do
        nil ->
          IO.puts "Player #{nickname} unknown."
        [x_position, y_position] ->
          x_pos_message = Integer.to_string(x_position)
          y_pos_message = Integer.to_string(y_position)
          message = "#{nickname} has X_Pos:#{x_pos_message} and Y_Pos #{y_pos_message}"
          :gen_tcp.send client_socket, message
        end
        route_messages(clients, clients_positions, game_choices)

      {:update_positions, nickname, new_x_position, new_y_position} ->
        clients_positions = Map.put(clients_positions, nickname, [new_x_position, new_y_position])

        values = Map.values(clients) -- [Map.get(clients, nickname)]
        send_fn = fn(value) -> :gen_tcp.send(value, "pos_#{nickname}_#{new_x_position}_#{new_y_position}_\n") end
        Enum.map(values, send_fn)

       route_messages(clients, clients_positions, game_choices)

      {:broadcast_message, message_body} ->
        for {_nickname, client_socket} <- clients do
            :gen_tcp.send client_socket, message_body
        end
      route_messages(clients, clients_positions, game_choices)

      {:challenge_player, nickname, other_nickname} ->

      :gen_tcp.send(clients[other_nickname], "challenged_#{nickname}_\n")

      route_messages(clients, clients_positions, game_choices)

      {:challenge_answer, player_who_challenged, choice} ->

      :gen_tcp.send(clients[player_who_challenged], "ChallengeAnswer_#{choice}_\n")

      route_messages(clients, clients_positions, game_choices)

      {:broadcast_status, nickname, other_nickname, status} ->

      values = Map.values(clients) -- [Map.get(clients, nickname)]
      send_fn = fn(value) -> :gen_tcp.send(value, "status_#{nickname}_#{other_nickname}_#{status}_\n") end
      Enum.map(values, send_fn)

      route_messages(clients, clients_positions, game_choices)

      {:broadcast_positions, nickname_of_receiver} ->
          client_socket_receiver = Map.get(clients, nickname_of_receiver)
            for{ nickname, [x_pos, y_pos]} <- clients_positions do
              x_pos_message = Integer.to_string(x_pos)
              y_pos_message = Integer.to_string(y_pos)
              message = "#{nickname} has X_Pos:#{x_pos_message} and Y_Pos #{y_pos_message}"
              :gen_tcp.send client_socket_receiver, message
          end
      route_messages(clients, clients_positions, game_choices)

      {:interpret_choice, nickname, choice} ->
        game_choices = Map.put(game_choices, nickname, choice)

        all_values = Map.values(game_choices)
        number_of_values = Enum.count(all_values)

        if(number_of_values > 1) do

            value1 = Map.get(game_choices, nickname)
            key1 = nickname

            key2_list = Map.keys(game_choices) -- [nickname]
            key2 = Enum.at(key2_list, 0)
            value2 = Map.get(game_choices, key2)

            if(value1 == value2)do
              :gen_tcp.send(clients[key1], "gameanswer_draw_\n")
              :gen_tcp.send(clients[key2], "gameanswer_draw_\n")

            else

                if( (value1 == 1 && value2 == 3) || (value1 == 2 && value2 ==1) || (value1 == 3 && value2 ==2) )do
                    :gen_tcp.send(clients[key1], "gameanswer_win_\n")
                    :gen_tcp.send(clients[key2], "gameanswer_lose_\n")

                else
                    :gen_tcp.send(clients[key1], "gameanswer_lose_\n")
                    :gen_tcp.send(clients[key2], "gameanswer_win_\n")
                  end

            end

        route_messages(clients, clients_positions, Map.new())

        end

        route_messages(clients, clients_positions, game_choices)


      {:register, nickname, client_socket, x , y} ->
        IO.puts "Player: '#{nickname}' has x position of:#{x} and y position of:#{y}"

        who_gets_registered = client_socket

        if(clients != %{}) do

            values = Map.values(clients)
            send_fn = fn(value) -> :gen_tcp.send(value, "register_#{nickname}_#{x}_#{y}_\n") end
            Enum.map(values, send_fn)

            for{ nickname_, [x_pos, y_pos]} <- clients_positions do
              x_pos_message = Integer.to_string(x_pos)
              y_pos_message = Integer.to_string(y_pos)

              :gen_tcp.send(who_gets_registered, "register_#{nickname_}_#{x_pos_message}_#{y_pos_message}_\n")
            end
         end

        route_messages(Map.put(clients, nickname, client_socket), Map.put(clients_positions, nickname, [x , y]), game_choices)

      {:unregister, nickname} ->
        route_messages(Map.delete(clients, nickname), Map.delete(clients_positions, nickname), game_choices)

      :shutdown ->
        IO.puts "Shutting down."

      anything_else ->
        IO.puts "Warning! Received:"
        IO.inspect anything_else
        route_messages(clients, clients_positions, game_choices)
    end
  end

end
