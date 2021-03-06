#include<routage_commande.h>


int file_i2c;
int length;
unsigned char buffer[9];



 
int main(void)
{
	
	/* Génération des variables de sockets nécessaire */
	struct sockaddr_in si_me, si_other;     
	int s, i, slen = sizeof(si_other) , recv_len;
	/* Buffer de réception wifi */
	char buf[BUFLEN];
     
	/* Création d'une socket UDP */
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
        	die("socket");
    	}
     
	/* Remplissage mémoire des structures par des zéros */
	memset((char *) &si_me, 0, sizeof(si_me));

	/* Initialisation des poramètres sockets */ 
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     

	/* Connection de la socket au port d'écoute */
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		die("bind");
	}
     

	/* Attente de réception de données wifi */
	while(1)
	{
        	printf("Waiting for data...");
        	fflush(stdout);
         
        	/* Appel bloquant : le programme continue d'écouter s'il n'a rien reçu */
        	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        	{
        		die("recvfrom()");
        	}
         
        	/* Affichage des données reçues ainsi que l'adresse IP/PORT de l'envoyeur */
        	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        	printf("Data: %s\n" , buf);
  
  
		/* Gestion de la communication I2C */

		/* Ouverture du bus I2C */
		char *filename = (char*)"/dev/i2c-1";
		if ((file_i2c = open(filename, O_RDWR)) < 0)
		{
			/* Erreur à l'ouverture du bus, vérifier errno pour plus détails */
			printf("Failed to open the i2c bus");
			return 0;
		}
		
		/* Adresse de l'esclave pour la prochaine communication I2C */	
		int addr = 0x20;         
		
		/* Début de la communication avec l'esclave */
		if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
		{
			printf("Failed to acquire bus access and/or talk to slave.\n");
			/* Erreur à la communication avec l'esclave, voir errno pour plus de détails */
			return 0;
		}
	

		/* Ecriture sur le bus I2C */	
		/* Longueur de la transmission à effectuer - doit correspondre à la tailel du buffer (en octet) */
		length = 9;		
		/* Write retourne le nombre d'octet écris, une erreur apparaît s'il ne correspond pas à la demande envoyée */ 
		if (write(file_i2c, buf, length) != length)		
		{
			/* Affichage d'une erreur de communication */
			printf("Failed to write to the i2c bus.\n");
		}
   

    		/* Réponse echo au client - A retravailler - non nécessaire */
        	if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        	{
            	die("sendto()");
        	}
	}

	/* Fermeture de la socket UDP */ 
	close(s);
	return 1;
}













	

















