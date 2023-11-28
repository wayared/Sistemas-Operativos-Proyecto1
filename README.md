# Operating Systems Project

Se requiere implementar una solución de monitoreo en tiempo real de los archivos logs de un sistema GNU/Linux. Se le ha pedido a usted esta implementación considerando las siguientes condiciones:

1. Proporcionar mediante parámetros lo siguiente:
> - Nombres de los servicios a monitorear (Mínimo 2).
> - El tiempo de actualización de la información (Lo puede definir como un parámetro o constante).
2. Considerar los niveles de prioridad tales como: 
> alert, err, notice, info, debug, etc.
3. Generar un dashboard en modo texto, que muestre la cantidad de mensajes generados de acuerdo al parámetro de actualización. Ejemplo de una solución web [Grafana Loki](https://www.youtube.com/watch?v=1obKa6UhlkY).
4. Para la lectura de los mensajes de acuerdo a la prioridad y servicio puede usar el comando [journalctl](https://www.loggly.com/ultimate-guide/using-journalctl/). Deberá usar la llamada del sistema exec.
5. Considerar definir una alerta que al superar un threshold pueda emitar un mensaje vía mensaje Whatsapp (Sugerencia API Twilio).
6. Finalmente deberá implementar un programa que genere estrés a uno de	los	servicios de uno de	los	clientes donde se pueda evidenciar la actualización	de los archivos log	para su	posterior notificación vía mensaje.

Se deberá tener	en ejecución 1 agente que se encuentren transmitiendo el estado	actual de los logs de mínimo 2 servicios y mediante	 1 prueba de estrés se evidencie la	actualización de los logs.

## Bibliografia

> [1] [Twilio docs](https://www.twilio.com/docs/usage/tutorials/how-to-use-your-free-trial-account)
> [2] [Journalctl docs](https://www.loggly.com/ultimate-guide/using-journalctl/)
> [3] [Example of Parsing Arguments with getopt](https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html)