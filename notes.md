# Client
- max 9 chars
-  nickname   =  ( letter / special ) *8( letter / digit / special / "-" ) no tengo ni idea de qye es *8

# Operator (esto no se si entra en el subject) 
- Puede conectar un nodo al servidor (no entra en el subject) (SQUIT && CONNECT)
- puede matar la conexion de un usuario (KILL)

# Channels
- 1 o mas de un cliente puede entrar a un canal
- Cuando el primer usuario entra se crea cuando el ultimo se va se destruye if (client[i].connect == 0) channel[i].destroy ??
- El nombre del canal tiene max 200 caracteres y empiezan por # y & no tiene <espacios> o <,>
- Comando JOIN para entrar a un canal o crearlo si el canal esta en modo +i solo podras entrar si el Channel Operator lo invita

# Channel operator
-chanop se llaman (lol)
> Por experiencia estos tienen una @ que se le agrega al ser el primero en entrar a un canal o si otro chanop se lo da pudiendo haber mas de un operador
> no se si esto hay que implementarlo no me leido el subject. Estoy siguiento la especificacion RFC 1459
- Comandos de chanop KICK() MODE( -> INVITE(+i) TOPIC(+t))

## Channel modes 
> Estos modos se cambian con el comando MODE
- o - give/take channel operator privileges;
- p - private channel flag;
- s - secret channel flag;
- i - invite-only channel flag;
- t - topic settable by channel operator only flag;
- n - no messages to channel from clients on the outside;
- m - moderated channel;
- l - set the user limit to channel;
- b - set a ban mask to keep users out;
- v - give/take the ability to speak on a moderated channel;
- k - set a channel key (password).

