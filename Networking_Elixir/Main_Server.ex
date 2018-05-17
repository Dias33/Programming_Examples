defmodule Server do
  use Application

  def start(_type, _args) do
    IO.puts("Server starting on ports 13000 and 13001")
    Task.start_link(Router, :start, [])
    Task.start_link(Tcp, :start, [13000])
  end
end

defmodule Tcp do

  def start(port) do
    options = [:binary, reuseaddr: true, active: false, backlog: 5]
    {:ok, lsocket} = :gen_tcp.listen(port, options)
    accept_loop(lsocket)
  end

  def accept_loop(lsocket) do
    case :gen_tcp.accept(lsocket) do
      {:ok, socket} ->
        spawn(fn -> client_loop(socket) end)
        accept_loop(lsocket)
      {:error, reason} ->
        IO.puts "tcp accept failed on:"
        IO.inspect(reason)
    end
  end

  def client_loop(socket) do

    case :gen_tcp.recv(socket,0) do
      {:ok, message} ->
        #IO.puts "Received #{message}"
        Message.parse(message, socket)
        client_loop(socket)
      {:error,why} ->
        IO.puts "Error: "
        IO.inspect why
        :gen_tcp.close(socket)

    end
  end

end


defmodule Message do
  def parse(msg, socket) do
    case String.split(msg, "_") do

      ["GameChoice", nickname, choice|_] ->
          int_choice = String.to_integer(choice)
          Router.interpret_choice(nickname, int_choice)

      ["Register",nickname, x_pos, y_pos|_] ->
            x = String.to_integer(x_pos)
            y = String.to_integer(y_pos)
          Router.register(nickname, socket, x, y)

      ["ChallengePlayer", nickname, other_nickname|_] ->
          Router.challenge_player(nickname, other_nickname)

      ["ChallengeAnswer", player_who_challenged, choice |_] ->
          Router.challenge_answer(player_who_challenged, choice)

      ["BroadcastStatus", _nickname, _other_nickname, _status, "ChallengePlayer", nickname_1, nickname_2|_] ->
          Router.challenge_player(nickname_1, nickname_2)

      ["BroadcastStatus", nickname, other_nickname, status|_] ->
          Router.broadcast_status(nickname, other_nickname, status)

      ["UpdatePositions", nickname, x_pos, y_pos|_] ->
          new_x = String.to_integer(x_pos)
          new_y = String.to_integer(y_pos)
          Router.update_positions(nickname, new_x, new_y)

      ["Whisper", addressee,message_body|_] ->
          Router.send_chat_message(message_body,addressee)

      ["GetPositions", nickname|_] ->
          Router.get_player_position(nickname, socket)

      ["BroadcastMessage:", message_body|_] ->
          Router.broadcast_message(message_body)

      ["UnRegister:", nickname|_] ->
          Router.unregister(nickname)

      default ->
          IO.puts "Warning ! Wrong message or format:"
          IO.inspect default

    end
  end
end
