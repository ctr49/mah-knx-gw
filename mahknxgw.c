#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <time.h>
#include <stdio.h>
#include <regex.h>
#include <stdio.h>

#define PORT 1609
#define MCGROUP "239.255.68.138"


static void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        print_element_names(cur_node->children);
    }
}

void main()
{
   struct sockaddr_in addr;
   int addrlen, sock, cnt, rv, i;
   struct ip_mreq mreq;
   char message[50], url[50], lang[16] = "?language=de_DE";
   char exp_c[10][50];
   regex_t exp[10];
   xmlDoc *doc = NULL;
   xmlNode *root_element = NULL;
   regmatch_t matches[1];

   strcpy(exp_c[0], "^http:\/\/[^\/]+:?\/homebus\/$"); // Type  0: Announce Gateway
   strcpy(exp_c[1], ""); // Type  x:
   strcpy(exp_c[2], ""); // Type  x:
   strcpy(exp_c[3], ""); // Type  x:
   strcpy(exp_c[4], ""); // Type  x:
   strcpy(exp_c[5], ""); // Type  x:
   strcpy(exp_c[6], ""); // Type  x:
   strcpy(exp_c[7], ""); // Type  x:
   strcpy(exp_c[8], ""); // Type  x:
   strcpy(exp_c[9], ""); // Type  x:

   for(i=0; i<10; i++) {
     rv = regcomp(&exp[i], exp_c[i], REG_EXTENDED);
     if (rv != 0) {
        printf("regcomp %d failed with %d\n", i, rv);
     }
   }

   /* set up socket */
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) {
     perror("socket");
     exit(1);
   }
   bzero((char *)&addr, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port = htons(PORT);
   addrlen = sizeof(addr);

      /* receive */
      if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {        
         perror("bind");
	 exit(1);
      }    
      mreq.imr_multiaddr.s_addr = inet_addr(MCGROUP);         
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
      if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		     &mreq, sizeof(mreq)) < 0) {
	 perror("setsockopt mreq");
	 exit(1);
      }         
      while (1) {
 	 cnt = recvfrom(sock, message, sizeof(message), 0, 
			(struct sockaddr *) &addr, &addrlen);
	 if (cnt < 0) {
	    perror("recvfrom");
	    exit(1);
	 } else if (cnt == 0) {
 	    break;
	 }
	 printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), message);
	 if (regexec(&exp[0], message, 1, matches, 0) == 0) {
		sprintf(url, "http://%s/homebus/", inet_ntoa(addr.sin_addr), lang);
		printf("url: %s\n", url);
         } else {
                printf("\"%s\" does not match\n", message);
         }




	    /*parse the file and get the DOM */
    doc = xmlReadFile(url, NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", url);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
        }
   for(i=0; i<10; i++) {
     regfree(&exp[i]);
   }
}


