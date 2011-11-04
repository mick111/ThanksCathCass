#include "MessageDB.h"
#include <time.h> 
#include <stdlib.h>

const char* const MESSAGES[] =
{       
        "legraslm_2011:Merci pour tout !",
        "bursauxa_2010:Une generation d'eleves se rappellera de votre disponibilite pour eux",
        "novelg_2012:Merci pour votre aide durant mon annee a l'Ensimag",
        "barbiert_2011:Merci pour tout!!! Profitez bien!",
        "muguett_2010:4d:65:72:63:69:20:70:6f:75:72:20:74:6f:75:74:20:21",
        "dietricj_2012:Au revoir CathCass, vous nous manquerez a tous. Profitez bien de votre retraite.",
        "guezr_2012:Bonne retraite!!",
        "polflies_2011:Merci pour votre travail extraordinaire ! Bonne continuation, profitez bien !",
        "pagliern_2012:Merci pour ces annees de bons et loyaux services envers l'Ensimag et ses eleves! Votre disponibilite sans faille etait tres appreciable!",
        "parentco_2010:fantastique travail ! tres bonne retraite !",
        "rouxca_2010:Merci pour ta patience, ta comprehension et ta gentillesse!",
        "besniera_2010:Je n'ai jamais fait d'usurpation d'identite !",
        "duponts_2011:Merci pour votre sourire et votre competence!",
        "khalifat_2012:Merci beaucoup pour toute l'aide apportee ! Vous avez fortement contribue a la formation des ingenieurs Ensimag.",
        "landrieb_2009:merci a une super admin ;) attention de ne pas trop abuser du sudo a la retraite, je vous souhaite bon vent pour votre retraite. Bises Viking",
        "repitonl_2010:-|- |-| /-\\ |\\| ><  |= () |2  /-\\ |_ |_ !!!",
        "coissars_2011:Merci pour tout, et bonne continuation.",
        "delefora_2010:Mes plus sinceres remerciements pour votre devouement, et votre contribution a ces agreables annees passees a l'Ensimag.",
        "rahroum_2010:Quand ca compile...c'est un peu grace a vous, Merci pour tout",
        "chellyo_2012:Sans votre devouement, mes deux dernieres annees auraient ete differentes.",
        "muelleri_2010:Vielen Dank fur alles und die besten Wunsche fur das weitere Leben! Grusse aus Deutschland!",
        "rousseas_2010:merci !",
        "duchenef_2010:Muchas gracias Mme root@montagnarde ;)",
        "royp_2011:Merci a vous pour ces trois annees (ainsi que toutes les precedentes...), pour votre support et votre amabilite ! Profitez de ce repos tant merite !",
        "aubertm_2011:Merci pour tout, tres bonne continuation !",
        "wang4_2011:xiexie (merci en chinois)",
        "loih_2012:Je me lance dans des experiences d'alchimie pour qu'un jour tous les informaticiens aient votre humour!",
        "ngot_2012:Au revoir et merci pour tout !",
        "thoonsem_2011:Vous etiez un peu la Trinity de l'Ensimag, le flingue en moins. Joyeuse retraite!",
        "reboudje_2012:Merci pour tout ce temps passe a reparer les erreurs des autres.",
        "sanchest_2012:une grande pensee pour vous! merci pour les fois ou vous m'avez sauve la vie quand il y avait des problemes de portes glissantes bloquees les samedi de \"rootage\". :)",
        "renaunil_2013:Merci pour les nombreuses reponses que vous nous avez fournies !",
        "edonf_2011:Merci pour tout, vous avez beaucoup apporte a toutes les promotions ENSIMAG !",
        "mougeyc_2013:Un grand MERCI pour votre comprehension et vos contributions dans la vie etudiante de l'ecole!",
        "faikm_2013:Je vous souhaite bonne continuation dans votre vie, je vous remercie du travail que vous avez accompli, en particulier l'aide que vous m'avez apporte lorsque j'ai perdu mes courriels :)",
        "louisthi_2012:Ah il va nous manquer, ton sourire bienveillant ! Mais t'as bien merite, de prendre du bon temps !",
        "champeaj_2011:Merci pour ces annees passees a l'imag a gerer tous nos problemes informatiques ! Bonne continuation dans la vie et bonne retraite, profitez en bien !  ",
        "siebmanb_2011:Merci pour le soutien sans faille au moindre probleme et pour nous avoir permis de travailler dans d'excellentes conditions !",
        "jouhris_2010:Je vous remercie infiniment pour tout le travail que vous avez fourni au sein de l'equipe informatique de notre chere ecole ENSIMAG et je vous souhaite bonne retraite pleine de joie et de bonheur.",
        "lesquera_2010:Merci!!!",
        "ngomn_2010:merci pour tout ce que vous avez fait pour nous",
        "lefurk_2012:Bonne continuation !",
        "zaimwadm_2012:Merci beaucoup CathCass pour tout ce que vous avez fait pour nous ! On vous sera toujours reconnaissants !",
        "cimonl_2012:Au revoir miss. Merci pour votre patience a toute epreuve et vos conseils chaleureux. Je me souviendrais de vous.",
        "eyraudj_2012:Merci et bonne continuation !",
        "vuntz_2003:Les salles TX, nos serveurs {telesun,ensibull,ensisun}, discussions avec vous... Merci pour ces excellents souvenirs !",
        "sraffi_2003:Merci pour tous ces souvenirs !",
        "roussilp_2012:Un enorme merci pour votre sympathie et votre ecoute, un bravo pour votre efficacite, et une tres bonne continuation a vous !",
        "bastider_2009:Merci pour toutes les fois ou vous avez sauvez nos comptes!!!",
        "abouchaz_2010:Merci beaucoup pour votre travail remarquable, votre humanite et tout ce que vous avez apporte a l'ecole",
        "lyonc_1993:Merci pour votre aide et votre confiance; bonne retraite!",
        "koeberla_2011:Merci pour tout ce que vous avez fait pour nous aider. Bonne retraite!",
        "mouchoum_2011:Merci enormement pour votre travail exemplaire dont vous avez fait preuve tout au long de notre scolarite. Encore desole pour mes fork bombs ! Bonne retraite!",
        "eddaoura_2012:Un GRAND MERCI a vous, felicitations pour cette longue carriere et pour cette retraite que vous avez amplement meritee!",
        "othmanj_2011:Merci pour tout!",
        "aufrerep_2012:Merci, profitez bien.",
        "tambourb_2011:Hi! je suis nouveau @ l'Ensimag, mais j'entends plein de bonnes choses sur vous :) Merciiii, au revoir et bon repos",
        "anzalaya_2010:export ANZALAYA=$ANZALAYA:\"Linux addict\" ; echo \"Merci\"",
        "frichote_2011:Merci pour votre serieux et votre disponibilite! Profitez bien de votre retraite bien meritee !!",
        "dejeand_2011:Merci Cath pour tout ce que vous nous avez apporte a l'ecole.",
        "badedikd_2010:Merci d'avoir ete presente quand j'ai eu besoin de vous.",
        "elabedm_2012:Merci ! Votre gentillesse et la qualite de vos competences vont beaucoup me manquer (et a toute l'Ensimag).Je vous souhaite une tres heureuse retraite",
        "fusader_2010:May the force be with you",
        "casatir_2011:Merci pour tout et bon vent !",
        "notheiss_2013:C'etait mieux avant! Merci pour tout."
};

const unsigned int NBMESSAGES = sizeof(MESSAGES)/sizeof(char *);

unsigned int g_auiOrdre[sizeof(MESSAGES)/sizeof(char *)] = {0};

void melange()
{
        /* Local Variables */
        unsigned int l_uiRandomNumber = 0;
        unsigned int l_uiTempNumber   = 0;
        unsigned int i                = 0;
        
        /* Randomization of random function */
	srand((unsigned int)time(NULL));
        
        /* Initialisation */
        for (i = 0; i < NBMESSAGES; i++)
        {
                g_auiOrdre[i] = i;
        }
        
        /* 5 swaps */
        for (i = 0; i < 5*NBMESSAGES; i++)
        {
                l_uiRandomNumber = (unsigned int) rand();
                l_uiTempNumber   = g_auiOrdre[l_uiRandomNumber%NBMESSAGES];
                g_auiOrdre[l_uiRandomNumber%NBMESSAGES] = g_auiOrdre[i%NBMESSAGES];
                g_auiOrdre[i%NBMESSAGES] = l_uiTempNumber;
        }
}
