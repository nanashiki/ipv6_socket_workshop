# POSIX Socket IPv4 -> IPv6ワークショップ

## IPv4でのサーバアプリ

1. v4ディレクトリに移動
```sh
cd v4
```
2. コード見てみる


3. コンパイル
```sh
g++ -o socket_server  socket_server.cpp
```

4. 起動

```sh
./socket_server
```

5. 確認

```sh
netstat -an | grep 12345

tcp4       0      0  *.12345                *.*                    LISTEN   
```

tcp4でIPv4のみでTCPでPORT 12345でLISTENしているのが確認できる

6. 叩いてみる

```sh
nc -zv 127.0.0.1 12345
nc -zv ::1  12345
nc -zv ::ffff:127.0.0.1  12345
```

or

```sh
telnet 127.0.0.1 12345
telnet ::1 12345
telnet ::ffff:127.0.0.1 12345
```

最近のmacOSはtelnet入ってないので `brew install telnet` が必要です

## IPv6に対応してみる

1. socket作成部分の対応

AF_INETがIPv4の意味でAF_INET6がIPv6の意味

`socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);` -> `socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);`

sockaddr_inはIPv4のアドレス構造体でIPv6のアドレスを入れられないので変更

`sockaddr_in` -> `sockaddr_in6`
`ddr.sin_family = AF_INET;` -> `addr.sin6_family = AF_INET6;`
` addr.sin_port = htons(12345);` -> `addr.sin6_port = htons(12345); `

2. bind部分の対応

対応なし

3. listen部分の対応

対応なし

4. accept部分の対応

`sockaddr_in` -> `sockaddr_in6`

5. 起動

```sh
g++ -o socket_server  socket_server.cpp
./socket_server
```

5. 確認

```
netstat -an | grep 12345
```

```sh
nc -zv 127.0.0.1 12345
nc -zv ::1  12345
nc -zv ::ffff:127.0.0.1  12345
```

or

```sh
telnet 127.0.0.1 12345
telnet ::1 12345
telnet ::ffff:127.0.0.1 12345
```

## IPv6のみにする

1. setsockoptの変更

`SOL_SOCKET` -> `IPPROTO_IPV6`
`SO_REUSEADDR` -> `IPV6_V6ONLY`

2. 起動

```sh
g++ -o socket_server  socket_server.cpp
./socket_server
```

3. 確認

```
netstat -an | grep 12345
```

```sh
nc -zv 127.0.0.1 12345
nc -zv ::1  12345
nc -zv ::ffff:127.0.0.1  12345
```

or

```sh
telnet 127.0.0.1 12345
telnet ::1 12345
telnet ::ffff:127.0.0.1 12345
```

## IPv4でのクライアントアプリ

1. v4ディレクトリに移動(すでにいる場合はスキップ)

```sh
cd v4
```

2. コード見てみる

3. コンパイル
```sh
g++ -o socket_client  socket_client.cpp
```

4. 起動

```sh
./socket_client
```

## クライアントアプリをIPv6対応してみる

gethostbynameがIPv4のみ(かつこれは1つのIPアドレスしか持ってこれない気がする)のでgetaddrinfoを使う

getaddrinfoは複数のIPアドレス取ってこれるのでこれを順番に成功するまで接続要求していく。そのためコードのdiffが大きいので今回のワークショップでは書き換えることはやらない

<img width="1501" alt="スクリーンショット 2023-12-22 13 19 22" src="https://github.com/nanashiki/ipv6_socket_workshop/assets/15953027/9d075caa-05cd-422c-9b7f-264a22687241">

1. コード見てみる

3. コンパイル
```sh
g++ -o socket_client  socket_client.cpp
```

4. 起動

```sh
./socket_client
```


