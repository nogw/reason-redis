open Opium;

let (let.await) = Lwt.bind;
let (let.map) = (v, f) => Lwt.map(f, v);

let host = "localhost";
let port = 6379;

let connection = Redis_lwt.Client.connect({host, port});

let not_found = Rock.Response.make(~status=`Not_found, ());

let get =
  App.get("/:key", req => {
    let key = Router.param(req, "key");

    let.await conn = connection;
    let.map response = Redis_lwt.Client.get(conn, key);

    switch (response) {
    | Some(v) => Response.of_plain_text(v)
    | None => not_found
    };
  });

let post =
  App.post("/:key/:value", req => {
    let key = Router.param(req, "key");
    let value = Router.param(req, "value");

    let.await conn = connection;
    let.map _success = Redis_lwt.Client.set(conn, key, value);

    Response.of_plain_text(key ++ ": " ++ value);
  });

let delete =
  App.delete("/:key", req => {
    let key = Router.param(req, "key");

    let.await conn = connection;
    let.await _success = Redis_lwt.Client.del(conn, [key]);

    Response.of_plain_text("DEL " ++ key) |> Lwt.return;
  });

let () = App.empty |> get |> post |> delete |> App.run_command |> ignore;
