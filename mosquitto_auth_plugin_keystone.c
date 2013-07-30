#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include <mosquitto.h>
#include <mosquitto_plugin.h>
#include "adcmb.h"
#ifdef __VMS
#pragma names save
#pragma names uppercase
#endif
#include "curl/curl.h"
#ifdef __VMS
#pragma names restore
#endif

#ifndef DEFAULT_URI
#define DEFAULT_URI "https://region-a.geo-1.identity.hpcloudsvc.com:35357/v2.0/tokens"
#endif

static char *keystone_uri = NULL;
static char *http_proxy = NULL;

#ifndef TMPLEN
#define TMPLEN 256
#endif


int mosquitto_auth_plugin_version(void)
{
	return (MOSQ_AUTH_PLUGIN_VERSION);
}

int mosquitto_auth_plugin_init(void **user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
	int 			i;

	for (i = 0; i < auth_opt_count; i++)
	{
#ifdef MQAP_DEBUG
	   fprintf(stderr, "AuthOptions: key=%s, val=%s\n", auth_opts[i].key, auth_opts[i].value);
#endif
	   if (strncmp(auth_opts[i].key, "keystone_uri", 12) == 0)
	   {
	      keystone_uri = auth_opts[i].value;
	   }
	   else if (strncmp(auth_opts[i].key, "http_proxy", 10) == 0)
	   {
	      http_proxy = auth_opts[i].value;
	   }
	}

	if (keystone_uri == NULL)
	{
	   keystone_uri = DEFAULT_URI;
	}

#ifdef MQAP_DEBUG
	fprintf(stderr, "URI = %s; proxy = %s\n", keystone_uri, http_proxy ? http_proxy : "null");
#endif
	return (MOSQ_ERR_SUCCESS);
}

int mosquitto_auth_plugin_cleanup(void *user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
	return (MOSQ_ERR_SUCCESS);
}

int mosquitto_auth_security_init(void *user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
	return (MOSQ_ERR_SUCCESS);
}

int mosquitto_auth_security_cleanup(void *user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
	return (MOSQ_ERR_SUCCESS);
}

int mosquitto_auth_acl_check(void *user_data, const char *clientid, const char *username, const char *topic, int access)
{
	/* For the moment we're assuming that if someone can log in then they can do all sorts of stuff */
	return (MOSQ_ERR_SUCCESS);
}

static int cb_write_func(char* data, size_t size, size_t num, adc_MB_t *mb)
{
	int 	      		len = size * num;

    	if (data != NULL)
    	{
	   assert((adc_MB_Append(mb, data, len) != -1));
    	}

	return (len);
}

int mosquitto_auth_unpwd_check(void *user_data, const char *username, const char *password)
{
	const char *		fmt = "{auth\":{\"passwordCredentials\":{\"username\":\"%s\", \"password\":\"%s\"}}}";

    	struct curl_slist *	headers = NULL;

	adc_MB_t * 		mb = NULL;

	char 			data[TMPLEN];

	int 			rc;
	int 			rv;

	CURL * 			ch;


	if (username == NULL || password == NULL)
	{
	   return (MOSQ_ERR_AUTH);
	}

	sprintf(data, fmt, username, password);

	if ((mb = adc_MB_New(0)) == NULL)
	{
#ifdef MQAP_DEBUG
	   fprintf(stderr, "malloc(): %s [%s, %d]\n", strerror(errno), __FILE__, __LINE__);
#endif
	   return (MOSQ_ERR_AUTH);
	}

	if ((ch = curl_easy_init()) == NULL)
	{
#ifdef MQAP_DEBUG
	   fprintf(stderr, "malloc(): %s [%s, %d]\n", strerror(errno), __FILE__, __LINE__);
#endif
	   return (MOSQ_ERR_AUTH);
	}

	/* Proxy may be NULL */
        curl_easy_setopt(ch, CURLOPT_PROXY, http_proxy);
	curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, cb_write_func);
	curl_easy_setopt(ch, CURLOPT_WRITEDATA, mb);

	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "Accept: application/json");

    	curl_easy_setopt(ch, CURLOPT_HTTPPOST, 1L);
    	curl_easy_setopt(ch, CURLOPT_URL, keystone_uri);
        curl_easy_setopt(ch, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(ch, CURLOPT_POSTFIELDSIZE, strlen(data));
	curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 0);

	if ((rv = curl_easy_perform(ch)) == CURLE_OK)
	{
	   curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &rc);

	   rv = rc;
	}
	else
	{
#ifdef MQAP_DEBUG
	   fprintf(stderr, "%s\n", curl_easy_strerror(rv));
#endif
	   rv = -1;
	}

    	curl_slist_free_all(headers);
	curl_easy_cleanup(ch);

	/* For the moment we don't really care what comes back in the body (TBD) */
   	adc_MB_Free(mb);

	if (rv == -1)
	{
	   return (MOSQ_ERR_AUTH);
	}

#ifdef MQAP_DEBUG
	if (rc != 200)
	{
	   fprintf(stderr, "HTTP response code = %d\n", rc);
	}
#endif

	return ((rc == 200 ? MOSQ_ERR_SUCCESS : MOSQ_ERR_AUTH));
}


int mosquitto_auth_psk_key_get(void *user_data, const char *hint, const char *identity, char *key, int max_key_len)
{
	return (MOSQ_ERR_AUTH);
}
