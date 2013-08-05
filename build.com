$! Grubby command procedure to build the plugin on OpenVMS...
$ set verify
$
$ define/job curl curl$root:[include]
$
$ cc/define=MQAP_DEBUG/nolist/names=(as_is,shortened)/include=([-.mosquitto-113.src],[-.mosquitto-113.lib]) mosquitto_auth_plugin_keystone.c
$ cc/nolist/names=(as_is,shortened) adcmb.c
$ link/share=mosquitto_auth_plugin_keystone.exe mosquitto_auth_plugin_keystone.obj,adcmb.obj,sys$input/opt
curl$root:[lib]curllib_hpssl.olb/lib
sys$library:ssl$libcrypto_shr32.exe/share
sys$library:ssl$libssl_shr32.exe/share
!
CASE_SENSITIVE=YES
SYMBOL_VECTOR=( -
	mosquitto_auth_plugin_version   = PROCEDURE, -
	mosquitto_auth_plugin_init      = PROCEDURE, -
	mosquitto_auth_plugin_cleanup   = PROCEDURE, -
	mosquitto_auth_security_init    = PROCEDURE, -
	mosquitto_auth_security_cleanup = PROCEDURE, -
	mosquitto_auth_acl_check        = PROCEDURE, -
	mosquitto_auth_unpwd_check      = PROCEDURE, -
	mosquitto_auth_psk_key_get      = PROCEDURE  -
)
!
$ purge/log
$ exit
