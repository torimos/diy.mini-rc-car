using System;
using System.Diagnostics;
using System.IO;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace rc_control
{
    class Program
    {
        static async Task Send(ClientWebSocket socket, string data) =>
            await socket.SendAsync(Encoding.UTF8.GetBytes(data), WebSocketMessageType.Text, true, CancellationToken.None);
        static async Task Receive(ClientWebSocket socket)
        {
            var buffer = new ArraySegment<byte>(new byte[2048]);
            do
            {
                WebSocketReceiveResult result;
                using (var ms = new MemoryStream())
                {
                    do
                    {
                        result = await socket.ReceiveAsync(buffer, CancellationToken.None);
                        ms.Write(buffer.Array, buffer.Offset, result.Count);
                    } while (!result.EndOfMessage);

                    if (result.MessageType == WebSocketMessageType.Close)
                        break;

                    ms.Seek(0, SeekOrigin.Begin);
                    using (var reader = new StreamReader(ms, Encoding.UTF8))
                        Console.WriteLine(await reader.ReadToEndAsync());
                }
            } while (true);
        }

        static async Task Update(ClientWebSocket socket, int speed, int steering)
        {
            const int s_offs = -10;
            await Send(socket, $"{steering + s_offs},{speed}");
            Console.WriteLine($"Sent: {steering},{speed}");
        }

        static async Task Main(string[] args)
        {
            int steering = 0, speed = 0, dir = 1;
            const int max_steer = 75;
            const int min_speed = 65;
            const int max_speed = 200;
            Stopwatch fb_steering = new Stopwatch();
            Stopwatch fb_speed = new Stopwatch();
            fb_steering.Start();
            fb_speed.Start();
            do
            {

                using (var socket = new ClientWebSocket())
                    try
                    {
                        await socket.ConnectAsync(new Uri("ws://10.0.1.22:8888/"), CancellationToken.None);
                        Console.WriteLine("Connected to rcc");
                        await Update(socket, speed, steering);
                        while (true)
                        {
                            if (Console.KeyAvailable)
                            {
                                var key = Console.ReadKey(true);

                                Console.WriteLine($"{key.Key}");

                                //TODO - fix single key pressed only issue by using some other method of 

                            //    if (key.Key == ConsoleKey.LeftArrow)
                            //    {
                            //        steering -= 5;
                            //        if (steering < -max_steer) steering = -max_steer;
                            //        fb_steering.Restart();
                            //    }
                            //    else if (key.Key == ConsoleKey.RightArrow)
                            //    {
                            //        steering += 5;
                            //        if (steering > max_steer) steering = max_steer;
                            //        fb_steering.Restart();
                            //    }

                            //    if (key.Key == ConsoleKey.A)
                            //    {
                            //        dir = 1;
                            //        Console.WriteLine($"Dir: {dir}");
                            //    }
                            //    else if (key.Key == ConsoleKey.Z)
                            //    {
                            //        dir = -1;
                            //        Console.WriteLine($"Dir: {dir}");
                            //    }

                            //    if (key.Key == ConsoleKey.UpArrow)
                            //    {
                            //        if (speed == 0)
                            //        {
                            //            await Update(socket, max_speed, steering);
                            //            await Task.Delay(120);
                            //            speed = min_speed;
                            //        }
                            //        speed += 10;
                            //        if (speed > max_speed) speed = max_speed;
                            //        fb_speed.Restart();
                            //    }
                            //    else if (key.Key == ConsoleKey.DownArrow)
                            //    {
                            //        speed -= 10;
                            //        if (speed < min_speed)
                            //        {
                            //            speed = 0;
                            //        }
                            //        fb_speed.Restart();
                            //    }

                            //    if (key.Key == ConsoleKey.Spacebar)
                            //    {
                            //        speed = 0;
                            //        steering = 0;
                            //    }
                            //    await Update(socket, speed * dir, steering);
                            }

                            //if (fb_speed.ElapsedMilliseconds > 500)
                            //{
                            //    speed = 0;
                            //    await Update(socket, speed * dir, steering);
                            //}
                            //if (fb_steering.ElapsedMilliseconds > 250)
                            //{
                            //    steering = 0;
                            //    await Update(socket, speed * dir, steering);
                            //}
                            //await Task.Delay(5);
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"ERROR - {ex.Message}");
                    }
            }
            while (true);
        }
    }
}
