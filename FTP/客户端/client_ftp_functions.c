#include "client_ftp.h"
#include"myStrtok.h"
static size_t size_packet = sizeof(struct packet);

static const char commandlist[NCOMMANDS][10] =
	{
		"get",
		"put",

		"mget",
		"mput",
		
		"cd",
		"lcd",
		
		"mgetwild",
		"mputwild",
		
		"dir",
		"ldir",

		"ls",
		"lls",
		
		"mkdir",
		"lmkdir",

		"rget",
		"rput",
		
		"pwd",
		"lpwd",
		
		"exit"
	};			// any change made here should also be \
				replicated accordingly in the COMMANDS \
				enum in commons.h

static void append_path(struct command* c, char* s)
{
	c->npaths++;
	char** temppaths = (char**) malloc(c->npaths * sizeof(char*));
	if(c->npaths > 1)
		memcpy(temppaths, c->paths, (c->npaths - 1) * sizeof(char*));

	char* temps = (char*) malloc((strlen(s) + 1) * sizeof(char));
	int i;
	for(i = 0; *(temps + i) = *(s + i) == '^' ? ' ' : *(s + i); i++)  //把 一个符号替换成空格
		;

	*(temppaths + c->npaths - 1) = temps;

	c->paths = temppaths;
}

//数据包格式：命令字,路径1,路径2.。。。。以此类推
struct command* userinputtocommand(char s[LENUSERINPUT])
{
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = -1;
	cmd->npaths = 0;
	cmd->paths = NULL;
	char* savestate;
	char token[10][10];
	for (int tmp = 0; tmp < 10; tmp++)
		for (int ttmp = 0; ttmp < 10; ttmp++)
			token[tmp][ttmp] = '\0';
	//for (int tmp = 0; tmp < 10; tmp++)
	//	*(token + tmp) = (char *)malloc(10 * sizeof(char *));
	int i, j;
	DivideStr(s, ',',token);
	for(i = 0; i<3; i++)
	//for (i = 0; s!=NULL; i++)
	//while(token = strtok_r(s, ",", &savestate)!=NULL)
	{
	
		//token = strtok_r(s, " \t\n", &savestate);
		//token = strtok_r(s, ",", &savestate);   //以 , 分隔命令字
	
		if(token == NULL)
			break;
		if(cmd->id == -1)
			for(j = 0; j < NCOMMANDS; j++)
			{	
				if(strcmp(token[0], commandlist[j])==0)
				{
					cmd->id = j;
					
					break;
				}
			}// ommitting braces for the "for loop" here is \
			 disastrous because the else below gets \
			 associated with the "if inside the for loop". \
			 #BUGFIX
		else
		{
			append_path(cmd, token[i]);
			
		}
	}
	printf("0 :%s\n", token[0]);
	printf("1 :%s\n", token[1]);
	printf("2 :%s\n", token[2]);

	//printf("0 :%s", token[0]);
	printf("path NUM:%d\n", cmd->npaths);
	printf("path0:%s\n", cmd->paths[0]);
	printf("path1:%s\n", cmd->paths[1]);
	if(cmd->id == MGET && !strcmp(*cmd->paths, "*"))
		cmd->id = MGETWILD;
	else if(cmd->id == MPUT && !strcmp(*cmd->paths, "*"))
		cmd->id = MPUTWILD;
	if(cmd->id != -1)
		return cmd;
	else
	{
		fprintf(stderr, "\tError parsing command\n");
		return NULL;
	}
}

void printcommand(struct command* c)
{
	if(!DEBUG)
		return;
	
	printf("\t\tPrinting contents of the above command...\n");
	printf("\t\tid = %d\n", c->id);
	printf("\t\tnpaths = %d\n", c->npaths);
	printf("\t\tpaths =\n");
	int i;
	for(i = 0; i < c->npaths; i++)
		printf("\t\t\t%s\n", c->paths[i]);
	printf("\n");
}

void command_pwd(struct packet* chp, struct packet* data, int sfd_client)
{
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = PWD;
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		er("send()", x);
	if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
		er("recv()", x);
	chp = ntohp(data);
	if(chp->type == DATA && chp->comid == PWD && strlen(chp->buffer) > 0)
		printf("\t%s\n", chp->buffer);
	else
		fprintf(stderr, "\tError retrieving information.\n");
}

void command_cd(struct packet* chp, struct packet* data, int sfd_client, char* path)
{
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = CD;
	strcpy_s(chp->buffer,sizeof(path), path);
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		er("send()", x);
	if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
		er("recv()", x);
	chp = ntohp(data);
	if(chp->type == INFO && chp->comid == CD && !strcmp(chp->buffer, "success"))
		;
	else
		fprintf(stderr, "\tError executing command on the server.\n");
}

void command_lls(char* lpwd)
{
	DIR* d = opendir(lpwd);
	if(!d)
		er("opendir()", (int) d);
	struct dirent* e;
	while(e = readdir(d))
		printf("\t%s\t%s\n", e->d_type == 4 ? "DIR:" : e->d_type == 8 ? "FILE:" : "UNDEF", e->d_name);
	closedir(d);
}

void command_ls(struct packet* chp, struct packet* data, int sfd_client)
{
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = LS;
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		//er("send()", x);
	while(chp->type != EOT)
	{
		if(chp->type == DATA && chp->comid == LS && strlen(chp->buffer))
			printf("\t%s\n", chp->buffer);
		/*
		else
			fprintf(stderr, "\tError executing command on the server.\n");
		*/
		if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
			er("recv()", x);
		chp = ntohp(data);
	}
}
//读写权限分为文本流和二进制流，注意区分，选择文本流则可能会导致文件损坏。
void command_get(struct packet* chp, struct packet* data, int sfd_client, char* filename)
{
	char url[128];
	char tmpName[128];
	for (int we = 0; we < 128; we++)
		{url[we] = '\0';
		 tmpName[we]='\0';
		}
	getcwd(url, sizeof url);  //获取当前文件路径
	for (int no = 128; no >= 0; no--)
	{
		if (filename[no] == '\\' || no == 0)
		{
			if (filename[no] == '\\') 
				no++;
			for (int j = 0; j < 128&&no<128; j++)
			{
				tmpName[j] = filename[no];
				no++;

			}
			break;
		}
	}                                      
	           //把真正的文件名挑出来
	//strcat_s(url, strlen(tmp)+1, tmp);
	url[strlen(url)] = '\\';
	//strcat_s(url, strlen(tmpName)+1, tmpName);
	int w = 0;
	int len = strlen(url);
	int maxlen = strlen(url) + strlen(tmpName);
	for (int q = len; q < maxlen; q++)
	{
		url[q] = tmpName[w];
		w++;
	}
	  //组装成绝对路径名。
	
	FILE* f;
	 fopen_s(&f,url, "wb+");
	 //printf("url:%s\n", url);
	if(!f)
	{
		fprintf(stderr, "File could not be opened for writing. Aborting...\n");
		return;
	}
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = GET;
	strcpy_s(chp->buffer, strlen(filename)+1,filename);
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		er("send()", x);
	if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
		er("recv()", x);
	chp = ntohp(data);
	//printpacket(chp, HP);
	if(chp->type == INFO && chp->comid == GET && strlen(chp->buffer))
	{
		printf("\t%s\n", chp->buffer);
		receive_file(chp, data, sfd_client, f);
		fclose(f);
	}
	else
		fprintf(stderr, "Error getting remote file : <%s>\n", filename);
}

void command_put(struct packet* chp, struct packet* data, int sfd_client, char* filename)
{
	FILE* f;
	 fopen_s(&f,filename, "rb");	// Yo!
	 printf("filename:%s\n", filename);
	if(!f)
	{
		fprintf(stderr, "File could not be opened for reading. Aborting...\n");
		return;
	}
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = PUT;
	strcpy_s(chp->buffer,strlen(filename)+1, filename);
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		er("send()", x);
	if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
		er("recv()", x);
	chp = ntohp(data);
	//printpacket(chp, HP);
	if(chp->type == INFO && chp->comid == PUT && strlen(chp->buffer))
	{
		printf("\t%s\n", chp->buffer);
		chp->type = DATA;
		send_file(chp, data, sfd_client, f);
		fclose(f);
	}
	else
		fprintf(stderr, "Error sending file.\n");
	send_EOT(chp, data, sfd_client);
}

void command_mget(struct packet* chp, struct packet* data, int sfd_client, int n, char** filenames)
{
	int i;
	char* filename;
	for(i = 0; i < n; i++)
	{
		filename = *(filenames + i);
		printf("\tProcessing file %d of %d:\t%s\n", i + 1, n, filename);
		command_get(chp, data, sfd_client, filename);
	}
	if(i != n)
		fprintf(stderr, "Not all files could be downloaded.\n");
}

void command_mput(struct packet* chp, struct packet* data, int sfd_client, int n, char** filenames)
{
	int i;
	char* filename;
	for(i = 0; i < n; i++)
	{
		filename = *(filenames + i);
		printf("\tProcessing file %d of %d:\t%s\n", i + 1, n, filename);
		command_put(chp, data, sfd_client, filename);
	}
	if(i != n)
		fprintf(stderr, "Not all files could be uploaded.\n");
}

void command_mgetwild(struct packet* chp, struct packet* data, int sfd_client)
{
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = LS;
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		er("send()", x);
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = MGETWILD;
	cmd->npaths = 0;
	cmd->paths = NULL;
	while(chp->type != EOT)
	{
		if(chp->type == DATA && chp->comid == LS && strlen(chp->buffer))
		if(*chp->buffer == 'F')
			append_path(cmd, chp->buffer + 6);
		if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
			er("recv()", x);
		chp = ntohp(data);
	}
	command_mget(chp, data, sfd_client, cmd->npaths, cmd->paths);
}

void command_mputwild(struct packet* chp, struct packet* data, int sfd_client, char* lpwd)
{
	DIR* d = opendir(lpwd);
	if(!d)
		er("opendir()", (int) d);
	struct dirent* e;
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = MPUTWILD;
	cmd->npaths = 0;
	cmd->paths = NULL;
	while(e = readdir(d))
		if(e->d_type == 8)
			append_path(cmd, e->d_name);
	closedir(d);
	command_mput(chp, data, sfd_client, cmd->npaths, cmd->paths);
}

void command_rput(struct packet* chp, struct packet* data, int sfd_client)
{
	static char lpwd[LENBUFFER];
	if(!getcwd(lpwd, sizeof lpwd))
		er("getcwd()", 0);
	int x;
	DIR* d = opendir(lpwd);
	if(!d)
		er("opendir()", (int) d);
	struct dirent* e;
	struct command* cmd = (struct command*) malloc(sizeof(struct command));
	cmd->id = RPUT;
	cmd->npaths = 0;
	cmd->paths = NULL;
	while(e = readdir(d))
		if(e->d_type == 8)
			append_path(cmd, e->d_name);
		else if(e->d_type == 4 && strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
		{
			command_mkdir(chp, data, sfd_client, e->d_name);
			
			command_cd(chp, data, sfd_client, e->d_name);
			command_lcd(e->d_name);
			
			command_rput(chp, data, sfd_client);
			
			command_cd(chp, data, sfd_client, "..");
			command_lcd("..");
		}
	closedir(d);
	command_mput(chp, data, sfd_client, cmd->npaths, cmd->paths);
}

void command_rget(struct packet* chp, struct packet* data, int sfd_client)
{
	char temp[LENBUFFER];
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = RGET;
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		er("send()", x);
	
	if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
		er("recv()", x);
	chp = ntohp(data);
	//printpacket(chp, HP);
	while(chp->type == REQU)
	{
		if(chp->comid == LMKDIR)
		{
			strcpy_s(temp, strlen((chp->buffer)+1),chp->buffer);
			command_lmkdir(temp);
		}
		else if(chp->comid == LCD)
		{
			strcpy_s(temp, strlen((chp->buffer))+1,chp->buffer);
			command_lcd(temp);
		}
		else if(chp->comid == GET)
		{
			strcpy_s(temp,strlen((chp->buffer))+1 ,chp->buffer);
			command_get(chp, data, sfd_client, temp);
		}

		if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
			er("recv()", x);
		chp = ntohp(data);
		//printpacket(chp, HP);
	}
	if(chp->type == EOT)
		printf("\tTransmission successfully ended.\n");
	else
		fprintf(stderr, "There was a problem completing the request.\n");
}

void command_mkdir(struct packet* chp, struct packet* data, int sfd_client, char* dirname)
{
	int x;
	set0(chp);
	chp->type = REQU;
	chp->conid = -1;
	chp->comid = MKDIR;
	strcpy_s(chp->buffer, strlen(dirname)+1,dirname);
	data = htonp(chp);
	if((x = send(sfd_client, data, size_packet, 0)) != size_packet)
		er("send()", x);
	if((x = recv(sfd_client, data, size_packet, 0)) <= 0)
		er("recv()", x);
	chp = ntohp(data);
	if(chp->type == INFO && chp->comid == MKDIR)
	{
		if(!strcmp(chp->buffer, "success"))
			printf("\tCreated directory on server.\n");
		else if(!strcmp(chp->buffer, "already exists"))
			printf("\tDirectory already exitst on server.\n");
	}
	else
		fprintf(stderr, "\tError executing command on the server.\n");
}

void command_lmkdir(char* dirname)
{
	DIR* d = opendir(dirname);
	if(d)
	{
		printf("\tDirectory already exists.\n");
		closedir(d);
	}
	else if(mkdir(dirname, 0777) == -1)
		fprintf(stderr, "Error in creating directory.\n");
	else
		printf("\tCreated directory.\n");
}

void command_lcd(char* path)
{
	if(chdir(path) == -1)
		fprintf(stderr, "Wrong path : <%s>\n", path);
}

