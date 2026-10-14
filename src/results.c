#include "results.h"
#include "mem_ops.h"
#define TEMPLATE_RES "/opt/0d1n-debian/templates/template.conf"
#define RESPONSE_PATH "/opt/0d1n-debian/view/response_sources"

static bool json_header_written = false;
static bool has_json_items = false;

void export_json_results(void) {
    if (param.json_file == NULL) {
        return;
    }

    /* Se o cabecalho ainda nao foi criado, gera o JSON completo */
    if (!json_header_written) {
        FILE *fjson = fopen(param.json_file, "w");
        if (!fjson) {
            printf("[!] Erro ao criar o arquivo JSON: %s\n", param.json_file);
            return;
        }
        fprintf(fjson, "{\n");
        fprintf(fjson, "  \"tool\": \"0d1n-debian\",\n");
        fprintf(fjson, "  \"host\": \"%s\",\n", param.host ? param.host : "");
        fprintf(fjson, "  \"payloads\": \"%s\",\n", param.payloads ? param.payloads : "");
        fprintf(fjson, "  \"delay_ms\": %d,\n", param.delay_ms);
        fprintf(fjson, "  \"results\": [\n  ]\n}\n");
        fclose(fjson);
        json_header_written = true;
        printf("\n[+] Relatorio JSON salvo em: %s\n", param.json_file);
        return;
    }

    /* Se ja foram inseridos itens, fecha o array e o objeto */
    FILE *fjson = fopen(param.json_file, "a");
    if (fjson) {
        fprintf(fjson, "\n  ]\n}\n");
        fclose(fjson);
        printf("\n[+] Relatorio JSON completo salvo em: %s\n", param.json_file);
    }
}

void append_json_item(long status, long length, const char *payload, const char *time_total) {
    if (param.json_file == NULL) return;

    /* Cria o cabecalho com inicio do array na primeira insercao */
    if (!json_header_written) {
        FILE *fjson = fopen(param.json_file, "w");
        if (!fjson) return;
        fprintf(fjson, "{\n");
        fprintf(fjson, "  \"tool\": \"0d1n-debian\",\n");
        fprintf(fjson, "  \"host\": \"%s\",\n", param.host ? param.host : "");
        fprintf(fjson, "  \"payloads\": \"%s\",\n", param.payloads ? param.payloads : "");
        fprintf(fjson, "  \"delay_ms\": %d,\n", param.delay_ms);
        fprintf(fjson, "  \"results\": [\n");
        fclose(fjson);
        json_header_written = true;
    }

    FILE *fjson = fopen(param.json_file, "a");
    if (fjson) {
        fprintf(fjson, "%s    {\n", has_json_items ? ",\n" : "");
        fprintf(fjson, "      \"status\": %ld,\n", status);
        fprintf(fjson, "      \"length\": %ld,\n", length);
        fprintf(fjson, "      \"payload\": \"%s\",\n", payload ? payload : "");
        fprintf(fjson, "      \"time_total\": \"%s\"\n", time_total ? time_total : "");
        fprintf(fjson, "    }");
        has_json_items = true;
        fclose(fjson);
    }
}

void write_result(
 char * chunk,
 char *pathtable,
 char *line,
 char * make, 
 char * make_agent, 
 char * make_cookie,
 long status,
 long length,
 char * time_total
)
{
    bool match_string=false; 
    char randname[32],log[3048],tabledata[4086],pathsource[1024],pathurl[1024];
    char *ptr_line = NULL, *delim = "\n", *tmp_list = NULL;

    /* Registra o item no JSON se o status for valido (diferente de 404) */
    if (status != 404) {
        append_json_item(status, length, line, time_total);
    }

    if(  (param.find_string_list) || (param.find_regex_list)  )
    {
        tmp_list = strdup(blob.buf_list);
        ptr_line = strtok(tmp_list, delim);

        while (ptr_line != NULL) 
        {
            char *tmp_make_cookie = NULL,*tmp_make_agent = NULL;
            char *tmp_make = NULL,*tmp_line = NULL,*tmp_ptr_line = NULL,*tmp_response = NULL,*responsetemplate = NULL;

            if (status)
            {
                if (param.find_string_list != NULL )
                    match_string = strstr(chunk,ptr_line)?true:false;

                if (param.find_regex_list != NULL )
                    match_string = strstr_regex(chunk,ptr_line)?true:false;
            }
            if (chunk && (match_string == true) ) 
            {
                if (make_cookie!=NULL)
                    fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Grep: %s %s %s  Params: %s \nCookie: %s \nTime: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,ptr_line,YELLOW,make,make_cookie,time_total,LAST);

                if (make_agent!=NULL)
                    fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Grep: %s %s %s  Params: %s \nCookie: %s\nTime: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,ptr_line,YELLOW,make,make_agent,time_total,LAST);
                else 
                    fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Grep: %s %s %s  Params: %s Time: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,ptr_line,YELLOW,make,time_total,LAST);

                if (param.save_response==true && status != 404)
                {
                    memset(pathsource,0,sizeof(char)*1023);
                    strlcat(pathsource,RESPONSE_PATH,45);
                    strlcat(pathsource,"/",47);
                    strlcat(pathsource,param.log, 78);
                    mkdir(pathsource,S_IRWXU|S_IRWXG|S_IRWXO);
                    entropy_clock();
                    char *str_tmp_rng = rand_str(randname, sizeof randname);
                    snprintf(pathsource,986,"%s/%s/%s.html",RESPONSE_PATH,param.log,str_tmp_rng);
                    snprintf(pathurl,986,"response_sources/%s/%s.html",param.log,str_tmp_rng);
                }

                if(status != 404)
                {
                    snprintf(log,3047,"[ %ld ] Payload: %s  Grep: %s Params: %s cookie: %s  UserAgent: %s Time: %s\n Path Response Source: %s\n",status,line,ptr_line,make,(make_cookie!=NULL)?make_cookie:" ",(make_agent!=NULL)?make_agent:" ",time_total,pathsource);
                    write_file(param.log,log);
                    memset(log,0,3047); 
                }

                if (param.save_response==true && status != 404)
                {
                    responsetemplate = NULL;
                    responsetemplate = read_lines(TEMPLATE_RES);
                    write_file(pathsource,responsetemplate);
                    memset(responsetemplate,0,strlen(responsetemplate)-1);
                    tmp_response = NULL;
                    tmp_response = html_entities(chunk);
                    write_file(pathsource,tmp_response);
                    write_file(pathsource,"</pre></html>");
                }

                tmp_make = html_entities(make);
                tmp_ptr_line = html_entities(ptr_line);
                tmp_line = html_entities(line);

                if (make_cookie!=NULL)
                {
                    tmp_make_cookie = html_entities(make_cookie);
                    snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s cookie: %s\",\"%s\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_cookie,tmp_ptr_line,tmp_line,time_total);
                    memset(tmp_make_cookie,0,strlen(tmp_make_cookie)-1);
                }

                if (make_agent!=NULL)
                {
                    tmp_make_agent = html_entities(make_agent);
                    snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s UserAgent: %s\",\"%s\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_agent,tmp_ptr_line,tmp_line,time_total);
                    memset(tmp_make_agent,0,strlen(tmp_make_agent)-1);
                } else 
                    snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s\",\"%s\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_ptr_line,tmp_line,time_total);

                if (status != 404)                  
                    write_file(pathtable,tabledata);

                XFREE(tmp_line);
                XFREE(tmp_make);
                XFREE(tmp_ptr_line);
                XFREE(responsetemplate);
                XFREE(tmp_response);    
                XFREE(tmp_make_cookie);
                XFREE(tmp_make_agent);
            }
            ptr_line = strtok(NULL, delim);
        }
        XFREE(tmp_list);

    } else {
        
        char *tmp_line = NULL,*tmp_make = NULL,*tmp_response = NULL,*responsetemplate = NULL;
        char *tmp_make_cookie = NULL,*tmp_make_agent = NULL;

        if (make_cookie != NULL)
            fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Params: %s %s\n Cookie: %s Time: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,make,make_cookie,time_total,LAST);
        
        if (make_agent != NULL)
            fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Params: %s %s\n UserAgent: %s Time: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,make,make_agent,time_total,LAST);
        else 
            fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Params: %s %s Time: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,make,time_total,LAST);

        if (param.save_response==true && status != 404)
        {       
            memset(pathsource,0,sizeof(char)*1023);
            strlcat(pathsource,RESPONSE_PATH,45);
            strlcat(pathsource,"/",46);
            strlcat(pathsource,param.log, 60);
            mkdir(pathsource,S_IRWXU|S_IRWXG|S_IRWXO);
            entropy_clock();
            char *str_tmp_rng = rand_str(randname, sizeof randname);
            snprintf(pathsource,986,"%s/%s/%s.html",RESPONSE_PATH,param.log,str_tmp_rng);
            snprintf(pathurl,986,"response_sources/%s/%s.html",param.log,str_tmp_rng);
        }

        if(status != 404)
        {
            snprintf(log,3047,"[%ld Payload: %s Params: %s Cookie: %s UserAgent: %s \nTime: %s\n Path Response Source: %s\n",status,line,make,(make_cookie!=NULL)?make_cookie:" ",(make_agent!=NULL)?make_agent:" ",time_total,pathsource);
            write_file(param.log,log);
            memset(log,0,3047);
        }

        if (param.save_response==true && status != 404)
        {
            responsetemplate=read_lines(TEMPLATE_RES);
            write_file(pathsource,responsetemplate);
            tmp_response = html_entities(chunk);
            write_file(pathsource,tmp_response);
            write_file(pathsource,"</pre></html>");
        }

        tmp_make = html_entities(make);
        tmp_line = html_entities(line);

        if (make_cookie != NULL)
        {
            tmp_make_cookie = html_entities(make_cookie);
            snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s  cookie: %s\",\"\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_cookie,tmp_line,time_total);
        }

        if (make_agent != NULL)
        {
            tmp_make_agent = html_entities(make_agent);
            snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s  UserAgent: %s\",\"\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_agent,tmp_line,time_total);
        } else 
            snprintf(tabledata,4047,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s\",\"\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_line,time_total);

        if (status != 404)
            write_file(pathtable,tabledata);

        XFREE(tmp_line);
        XFREE(tmp_make);
        XFREE(tmp_response);
        XFREE(responsetemplate);
        XFREE(tmp_make_cookie);
        XFREE(tmp_make_agent);
    }
}