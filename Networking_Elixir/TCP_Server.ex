defmodule TCP do

  def server(port, pending_connections) do
    {:ok, listening_socket} = :gen_tcp.listen(port, [:binary,
          backlog: pending_connections,
          active: false, packet: :line])
    loop(listening_socket)
  end

def loop(listening_socket) do
  case :gen_tcp.accept(listening_socket) do
    {:ok,client_socket} ->
       spawn(fn -> client_loop(client_socket) end)
       loop(listening_socket)
    {:error,why} ->
      IO.puts "Error: "
      IO.inspect why
  end
end

def client_loop(client_socket) do
  case :gen_tcp.recv(client_socket,0) do
    {:ok, message} ->
      case message do

        <<"Whisper:", res::binary>> ->
              [addressee,message_body|_]= String.split(res, "_")
              Router.send_chat_message(message_body,addressee)
          client_loop(client_socket)

        <<"GetPositions:", res::binary>> ->
          [nickname|_]= String.split(res, "_")
          Router.get_player_position(nickname, client_socket)
          client_loop(client_socket)

        <<"UpdatePositions:", res::binary>> ->
            [nickname, x_pos, y_pos|_] = String.split(res, "_")
            new_x = String.to_integer(x_pos)
            new_y = String.to_integer(y_pos)
            Router.update_positions(nickname, new_x, new_y)
          client_loop(client_socket)

        <<"BroadcastMessage:", res::binary>> ->
              [message_body|_]= String.split(res, "_")
              Router.broadcast_message(message_body)
          client_loop(client_socket)

        <<"BroadcastPositions:", res::binary>> ->
            [nickname|_] = String.split(res, "_")
            Router.broadcast_positions(nickname)
          client_loop(client_socket)

        <<"Register:", res::binary>> ->
              [nickname, x_pos, y_pos|_] = String.split(res, "_")
              x = String.to_integer(x_pos)
              y = String.to_integer(y_pos)
              Router.register(nickname, client_socket, x, y)
          client_loop(client_socket)

        <<"UnRegister:", res::binary>> ->
              [nickname|_] = String.split(res, "_")
              Router.unregister(nickname)
          client_loop(client_socket)

        default ->
                    IO.puts "Warning ! Wrong message or format:"
                    IO.inspect default
          client_loop(client_socket)

      end
      # :gen_tcp.send(client_socket,message)
      client_loop(client_socket)
    {:error,why} ->
      IO.puts "Error: "
      IO.inspect why
      :gen_tcp.close(client_socket)
  end
end



end
