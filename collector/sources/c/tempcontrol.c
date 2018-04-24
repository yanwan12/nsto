//tempcontrol.c
//2014/11/13

unsigned char ucLogsForHeat[][LEN_FRAME_SERIAL_NB] = 
{{"LOG", 0x00, 0x00, 0x00},
 {"LGR", 0x00, 0x00, 0x00}};

float getThreshold()
{
	return 20.0;
}


float calculTemp(
	pstFrame _pst_frame
)
{
	float	fTemp = _pst_frame->acHexData[0] << 8 + _pst_frame-acHexData[1];

	switch(_pst_frame->)
	{
		case LOG26:
		case LOG22:
			fTemp *= 200;
			fTemp /= 2047;
			fTemp += 50;
			break;
		case LGR32:
		case LGR54:
		case LGR64:
			fTemp /= 100;
			break;
		default:
			break;
	}
	return fTemp;
}


void tempControl(
	pstFrame _pst_frame
)
{
	//mesure sans restitution = LIVE ?
	if ((_pst_frame->cPi0 & MASK_SUBTAG_RESTITUTION) == 0x00)
	{
		//logger dans la liste des loggers utilisés pour le chauffage?
		int iNbLogs = sizeof(ucLogsForHeat)/LEN_FRAME_HEX_SERIAL_NB;
		for (int i = 0; i < iNbLogs; i++)
		{
			for (int j = 0; j < LEN_FRAME_HEX_SERIAL_NB; j++)
			{
				if(_pst_frame->acSourceSnHex[j], != ucLogsForHeat[i][j])
				{
					break;
				}
			}
			if (j == LEN_FRAME_HEX_SERIAL_NB)
			{
				break;
			}
		}
		if (i < iNbLogs)
		{
			//Logger trouvé! Est-ce que la température est trop basse?
			float fTemp = calcuTemp(_pst_frame);
			if (fTemp < getThreshold())
			{
				chauffage(ON);
			}
		}
	}
}

