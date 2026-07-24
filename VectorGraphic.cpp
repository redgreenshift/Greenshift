#include "Project Greenshift.h"
/*
 *  Copyright (C) 2001-2026 Jared Ivey
 *
 *  This file is part of Project Greenshift
 *
 *  OSI Certified Open Source Software
 *
 *  Project Greenshift is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; version 2 only.
 *
 *  Project Greenshift is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

 /****************************************************************************
  *
  * Particle
  *
  ****************************************************************************/

#include "VectorGraphic.h"


  /****************************************************************************
   *
   * VectorGraphic - default constructor
   *
   ****************************************************************************/
VectorGraphic::VectorGraphic()
{
	m_pPen = NULL;
	m_pLineWidth = NULL;
	m_bConnected = false;
	m_dwResolution = 1;
	m_nResolution = 1.0f;
	//    m_nStep            = 0.0f;

	m_dwMode = 0;
	//    m_nX               = 0.0f;
	//    m_nZ               = 0.0f;
	m_pLastX = NULL;
	m_pLastY = NULL;
	m_nPointOfView = 20.0f;

	m_pAngleX = NULL;
	m_pAngleY = NULL;
	m_pAngleZ = NULL;

	m_nScale = 1.0f;

	m_strRotationOrder = NULL;
}

/****************************************************************************
 *
 * ~Particle - default destructor
 *
 ****************************************************************************/
VectorGraphic::~VectorGraphic()
{
	SAFE_FREE(m_strRotationOrder);

	SAFE_DELETE(m_pAngleX);
	SAFE_DELETE(m_pAngleY);
	SAFE_DELETE(m_pAngleZ);

	SAFE_DELETE(m_pLastX);
	SAFE_DELETE(m_pPen);
	SAFE_DELETE(m_pLineWidth);
}




/****************************************************************************
 *
 * Reset - allows reinitialization of values that do not change during
 *    particle lifetime.  Mainly used to reevaluate the A vars when starting
 *
 ****************************************************************************/
void    VectorGraphic::Reset(void)
{
	error_t err;

	err = Evaluate_A_Vars();

#if EXTREME_DEBUGGING
	if (err != SUCCESS)
	{
		DumpToFile("error.txt", ErrorString(err), "\n");
		MessageBox(NULL, "Reset failed!", "\n", MB_OK);
	}
#endif
}



/****************************************************************************
 *
 * Initialize - two stage initialization (USE PARTIAL SIMPLIFICATION!!!!)
 *
 ****************************************************************************/
error_t    VectorGraphic::Initialize(MyDictionary<mychar_t*>* inConfig,
	MyDictionary<value_t*>* inValues,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	error_t                 err;
	value_t                 nDefaultSteps;
	EXPRESSIONDESCRIPTION* edDefaultSteps;
	value_t                 nTmp;


	/*
	 * shouldn't have to assume variable names, etc...
	 */
	m_dValues.SetAlternate(inValues);
	if ((err = m_dValues.SetValue("NUM_S_STEPS", &m_nResolution)) != SUCCESS
		|| (err = m_dValues.SetValue("POV", &m_nPointOfView)) != SUCCESS
		)
	{
		return err;
	}

	nTmp = (value_t)Expression::Evaluate(inConfig->GetValue("POV"), 20.0f,
		&m_dValues, inGlobals);
	if (nTmp == 0.0f)
		m_nPointOfView = 20.0f;
	else
		m_nPointOfView = nTmp;


	nTmp = (value_t)Expression::Evaluate(inConfig->GetValue("Mode"),
		VG_PARAMETRIC, &m_dValues, inGlobals);
	if (nTmp < 0.0f)
		m_dwMode = 0;
	else
		m_dwMode = (DWORD)nTmp;


	/*
	 * initialize the PhaseFunction
	 */
	switch (m_dwMode)
	{
	default:
	case VG_PARAMETRIC:
		err = m_pfValues.Initialize("ABCD", "XYZ", inConfig, &m_dValues, inGlobals);
		break;
	case VG_4D:
		err = m_pfValues.Initialize("ABCD", "Y", inConfig, &m_dValues, inGlobals);

		if (err == SUCCESS)
			err = Expression::Compile(inConfig->GetValue("x_rotation"),//"-pi/4"),
				&m_pAngleX, &m_dValues, inGlobals);
		if (err == SUCCESS)
			err = Expression::Compile(inConfig->GetValue("y_rotation"),//, "pi/4"),
				&m_pAngleY, &m_dValues, inGlobals);
		if (err == SUCCESS)
			err = Expression::Compile(inConfig->GetValue("z_rotation"),//, "0"),
				&m_pAngleZ, &m_dValues, inGlobals);
		if (err == SUCCESS)
			m_strRotationOrder = strdup(inConfig->GetValue("rotation_order"));//, "zxy" ) );
		break;
	}
	if (err != SUCCESS)
		return err;


	Evaluate_A_Vars();  /* I know the other things can depend on the A vars,
						 * but it also seems the A vars can depend on other
						 * things
						 */


						 /*
						  * get the default number of steps
						  */
	switch (m_dwMode)
	{
	case VG_PARAMETRIC:
		edDefaultSteps = inGlobals->GetValue("Parametric_steps");
		break;

	case VG_4D:
		edDefaultSteps = inGlobals->GetValue("FourD_steps");
		break;

	default:
		edDefaultSteps = NULL;
		break;
	}

	if (edDefaultSteps != NULL && edDefaultSteps->edtType == ED_CONSTANT)
		nDefaultSteps = edDefaultSteps->value;
	else
		nDefaultSteps = 1.0f;

	//        nDefaultSteps = 1.0f;


		/*
		 * get NUM_S_STEPS, what is a good default value?
		 */
	m_nResolution = (value_t)Expression::Evaluate(inConfig->GetValue("Steps"), nDefaultSteps, &m_dValues, inGlobals);

	if (m_nResolution <= 0.0f) /* what does negative step mean?! */
		m_dwResolution = (DWORD)nDefaultSteps;
	else
		m_dwResolution = (DWORD)floor(m_nResolution);

	m_nResolution = (value_t)m_dwResolution;



	/***********************************

		 DON'T NEED THE CACHE IF NOT CONNECTED!!!

		ALSO, DON'T NEED TO *DRAW* MORE THAN ONE DOT WHEN NOT CONNECTED!!!

	  **********************************/




	  /*
	   * get connect the dots?
	   */
	m_bConnected = (atol(inConfig->GetValue("Connected", "0")) == 0) ? false : true;


	/*
	 * allocate clast X row cache
	 */
	if (m_dwMode == VG_4D)
	{
		if ((m_pLastX = new value_t[m_dwResolution * 2]) == NULL)
			return ERR_MALLOC;

		m_pLastY = &m_pLastX[m_dwResolution];
	}





	/*
	 * get Pen
	 */
	err = Expression::Compile(inConfig->GetValue("PenColor", "1"), &m_pPen, &m_dValues, inGlobals);
	if (err != SUCCESS)
		return err;


	/*
	 * get line width
	 */
	err = Expression::Compile(inConfig->GetValue("LineWidth", "1"), &m_pLineWidth, &m_dValues, inGlobals);
	if (err != SUCCESS)
		return err;


	return SUCCESS;
}


/****************************************************************************
 *
 * EvaluateXY_Pair -
 *
 ****************************************************************************/
void    VectorGraphic::EvaluateXY_Pair(const DWORD dwIndex, value_t* outX, value_t* outY)
{
	Evaluate_C_Vars();  /* C# vars - once per function */

	Evaluate_D_Vars();  /* D# vars - before every evaluation */
	*outX = m_pfValues.EvaluateFunction(dwIndex, 0);

	//    Evaluate_D_Vars();  /* D# vars - before every evaluation */
	*outY = m_pfValues.EvaluateFunction(dwIndex, 1);
}



#ifdef UNDEFINED
/****************************************************************************
 *
 * DrawFrameOn -
 *
 ****************************************************************************/
void    VectorGraphic::DrawFrameOn(Graph* lpGraph)
{
	switch (m_dwMode)
	{
	default:
	case VG_PARAMETRIC:
		switch (m_pfValues.NumDimensions())
		{
		case 2:
			Draw2dFrameOn(lpGraph);
			break;
		case 3:
			Draw3dFrameOn(lpGraph);
			break;
		default:
#if EXTREME_DEBUGGING
			DumpToFile("error.txt", "invalid dimension", "\n");
#endif
			break;
		}
		break; /* case 0 */

	case VG_4D:
		if (m_bConnected)
			Draw3dFrameOn2Connected(lpGraph);
		else
			Draw3dFrameOn2(lpGraph);
		break;
	}
}


/****************************************************************************
 *
 * Draw2dFrameOn -
 *
 ****************************************************************************/
void    VectorGraphic::Draw2dFrameOn(Graph* lpGraph)
{
	const value_t   nInverseResolution = 1.0f / (m_dwResolution - 1);
	value_t         nLastX;
	value_t         nLastY;
	value_t         nCurrentX;
	value_t         nCurrentY;
	value_t         crColor;
	value_t         nLineWidth;
	DWORD           i;
	DWORD           nFunction;


	m_nStep = 0.0f;

	Evaluate_B_Vars();  /* B# vars - once per frame */

	for (nFunction = 0; nFunction < m_pfValues.NumFunctions(); nFunction++)
	{
		m_nStep = 0.0f;

		EvaluateXY_Pair(nFunction, &nCurrentX, &nCurrentY);

		/* huh?  I thought I had to start at 1 */
		for (i = 0; i < m_dwResolution; i++)
		{
			//            m_nStep = (value_t)i / (value_t)(m_dwResolution-1);  /* set s */
			m_nStep = i * nInverseResolution;  /* set s */


			nLastX = nCurrentX;
			nLastY = nCurrentY;

			EvaluateXY_Pair(nFunction, &nCurrentX, &nCurrentY);

			//            DumpToFile( "error.txt", nCurrentX, " " );
			//            DumpToFile( "error.txt", nCurrentY, " " );

			crColor = m_pPen->Evaluate();


			nLineWidth = m_pLineWidth->Evaluate();

			if (m_bConnected)
				lpGraph->DrawLine(nLastX, -nLastY, nCurrentX, -nCurrentY, nLineWidth, crColor);
			else
				lpGraph->DrawDot(nCurrentX, -nCurrentY, nLineWidth, crColor);

		} /* step loop */

	} /* function loop */
}

#endif




/****************************************************************************
 *
 * EvaluateXYZ_Tuple -
 *
 ****************************************************************************/
void    VectorGraphic::EvaluateXYZ_Tuple(const DWORD dwIndex, value_t* outX, value_t* outY, value_t* outZ)
{
	Evaluate_C_Vars();  /* C# vars - once per function */

	Evaluate_D_Vars();  /* D# vars - before every evaluation */
	*outX = m_pfValues.EvaluateFunction(dwIndex, 0);

	//    Evaluate_D_Vars();  /* D# vars - before every evaluation */
	*outY = m_pfValues.EvaluateFunction(dwIndex, 1);

	//    Evaluate_D_Vars();  /* D# vars - before every evaluation */
	*outZ = m_pfValues.EvaluateFunction(dwIndex, 2);
}


/****************************************************************************
 *
 * ConvertTo2D -
 *
 ****************************************************************************/
 /*void    VectorGraphic::ConvertTo2d( const value_t x,
									 const value_t y,
									 const value_t z,
									 value_t *outX,
									 value_t *outY )
 {
	 const value_t nDepthPerceptionFactor
					 = m_nPointOfView / (m_nPointOfView - z);

	 *outX = x * nDepthPerceptionFactor;
	 *outY = y * nDepthPerceptionFactor;
 }
 /***/

#ifdef UNDEFINED
 /****************************************************************************
  *
  * Draw3dFrameOn -
  *
  ****************************************************************************/
void    VectorGraphic::Draw3dFrameOn(Graph* lpGraph)
{
	const value_t   nResolutionInverse = 1.0f / (m_dwResolution - 1);
	value_t         nLastX;
	value_t         nLastY;
	value_t         nCurrentX;
	value_t         nCurrentY;

	value_t         nX3dRaw;
	value_t         nY3dRaw;
	value_t         nZ3dRaw;

	value_t         nX3d;
	value_t         nY3d;
	value_t         nZ3d;
	DWORD           dwS;
	DWORD           dwFunction;


	m_nStep = 0.0f;

	Evaluate_B_Vars();  /* B# vars - once per frame */

	for (dwFunction = 0; dwFunction < m_pfValues.NumFunctions(); dwFunction++)
	{
		m_nStep = 0.0f;

		EvaluateXYZ_Tuple(dwFunction, &nX3dRaw, &nY3dRaw, &nZ3dRaw);
		m_pr3dRotator.RotatePoint(nX3dRaw, nY3dRaw, nZ3dRaw,
			&nX3d, &nY3d, &nZ3d);
		ConvertTo2d(nX3d, nY3d, nZ3d, &nCurrentX, &nCurrentY);

		/* huh?  I thought I could start at 1 */
		for (dwS = 0; dwS < m_dwResolution; dwS++)
		{
			m_nStep = dwS * nResolutionInverse;  /* set s */


			nLastX = nCurrentX;
			nLastY = nCurrentY;

			EvaluateXYZ_Tuple(dwFunction, &nX3dRaw, &nY3dRaw, &nZ3dRaw);
			m_pr3dRotator.RotatePoint(nX3dRaw, nY3dRaw, nZ3dRaw,
				&nX3d, &nY3d, &nZ3d);
			ConvertTo2d(nX3d, nY3d, nZ3d, &nCurrentX, &nCurrentY);

			if (m_bConnected)
				DrawLine(nLastX, -nLastY, nCurrentX, -nCurrentY, lpGraph);
			else
				lpGraph->DrawDot(nCurrentX, -nCurrentY,
					m_pLineWidth->Evaluate(), m_pPen->Evaluate());

		} /* step loop */

	} /* function loop */
}

#endif





/****************************************************************************
 *
 * EvaluateRotation - create the rotation matrix
 *
 ****************************************************************************/
void    VectorGraphic::EvaluateRotation(void)
{
	DWORD    i;

	m_pr3dRotator.ResetToIdentity();

	if (m_strRotationOrder != NULL)
		for (i = 0; i < strlen(m_strRotationOrder); i++)
		{
			switch (tolower(m_strRotationOrder[i]))
			{
			case 'x':
				m_pr3dRotator.RotateX(m_pAngleX->Evaluate());
				break;
			case 'y':
				m_pr3dRotator.RotateY(m_pAngleY->Evaluate());
				break;
			case 'z':
				m_pr3dRotator.RotateZ(m_pAngleZ->Evaluate());
				break;
			default:
				break;
			}
		}

}


#ifdef UNDEFINED
/****************************************************************************
 *
 * Draw3dFrameOn2 - the Mode 1 version, needs optimization!
 *
 ****************************************************************************/
void    VectorGraphic::Draw3dFrameOn2(Graph* lpGraph)
{
	const value_t   nResolutionInverse = 2.0f / (m_dwResolution - 1);
	value_t         nCurrentX;
	value_t         nCurrentY;

	value_t         m_nY;

	value_t         nX3d;
	value_t         nY3d;
	value_t         nZ3d;
	DWORD           dwX;
	DWORD           dwZ;
	DWORD           dwFunction;


	m_nX = -1.0f;
	m_nZ = -1.0f;

	Evaluate_B_Vars();  /* B# vars - once per frame */

	EvaluateRotation(); /* reevaluate the rotation */

	for (dwFunction = 0; dwFunction < m_pfValues.NumFunctions(); dwFunction++)
	{
		/* Z axis loop */
		for (dwZ = 0; dwZ < m_dwResolution; dwZ++)
		{
			m_nZ = dwZ * nResolutionInverse - 1.0f;  /* set z */;

			/* X axis loop */
			for (dwX = 0; dwX < m_dwResolution; dwX++)
			{
				m_nX = dwX * nResolutionInverse - 1.0f;  /* set x */;
				m_nY = m_pfValues.EvaluateFunction(dwFunction, 0);
				m_nStep = (value_t)sqrt(m_nX * m_nX + m_nZ * m_nZ);


				m_pr3dRotator.RotatePoint(m_nX, m_nY, m_nZ,
					&nX3d, &nY3d, &nZ3d);
				ConvertTo2d(nX3d, nY3d, nZ3d, &nCurrentX, &nCurrentY);

				lpGraph->DrawDot(nCurrentX, -nCurrentY,
					m_pLineWidth->Evaluate(),
					m_pPen->Evaluate());

			}/* X loop */

		} /* Z loop */


	} /* function loop */
}



/****************************************************************************
 *
 * Draw3dFrameOn2 - the Mode 1 version, needs optimization!
 *
 ****************************************************************************/
void    VectorGraphic::Draw3dFrameOn2Connected(Graph* lpGraph)
{
	const value_t   nResolutionInverse = 2.0f / (m_dwResolution - 1);
	value_t         nLastX;
	value_t         nLastY;
	value_t         nCurrentX;
	value_t         nCurrentY;

	value_t         m_nY;

	value_t         nX3d;
	value_t         nY3d;
	value_t         nZ3d;
	DWORD           dwX;
	DWORD           dwZ;
	DWORD           dwFunction;


	m_nX = -1.0f;
	m_nZ = -1.0f;

	Evaluate_B_Vars();  /* B# vars - once per frame */

	EvaluateRotation();

	for (dwFunction = 0; dwFunction < m_pfValues.NumFunctions(); dwFunction++)
	{
		m_nX = -1.0f;
		m_nZ = -1.0f;
		m_nY = m_pfValues.EvaluateFunction(dwFunction, 0);
		m_nStep = (value_t)sqrt(m_nX * m_nX + m_nZ * m_nZ);


		m_pr3dRotator.RotatePoint(m_nX, m_nY, m_nZ, &nX3d, &nY3d, &nZ3d);
		ConvertTo2d(nX3d, nY3d, nZ3d, &nCurrentX, &nCurrentY);

		m_pLastX[0] = nCurrentX;
		m_pLastY[0] = nCurrentY;

		/*
		 * initial X axis loop
		 */
		 /* huh?  I thought I had to start at 1 */
		for (dwX = 0; dwX < m_dwResolution; dwX++)
		{
			m_nX = dwX * nResolutionInverse - 1.0f;  /* set x */;
			m_nZ = -1.0f;
			m_nY = m_pfValues.EvaluateFunction(dwFunction, 0);
			m_nStep = (value_t)sqrt(m_nX * m_nX + m_nZ * m_nZ);

			nLastX = nCurrentX;
			nLastY = nCurrentY;

			m_pr3dRotator.RotatePoint(m_nX, m_nY, m_nZ, &nX3d, &nY3d, &nZ3d);
			ConvertTo2d(nX3d, nY3d, nZ3d, &nCurrentX, &nCurrentY);


			DrawLine(nLastX, -nLastY, nCurrentX, -nCurrentY, lpGraph);

			m_pLastX[dwX] = nCurrentX;
			m_pLastY[dwX] = nCurrentY;
		} /* X preloop */



		/*
		 * Z axis loop
		 */
		for (dwZ = 1; dwZ < m_dwResolution; dwZ++)
		{
			m_nX = -1.0f;
			m_nZ = dwZ * nResolutionInverse - 1.0f;  /* set z */;
			m_nY = m_pfValues.EvaluateFunction(dwFunction, 0);
			m_nStep = (value_t)sqrt(m_nX * m_nX + m_nZ * m_nZ);

			m_pr3dRotator.RotatePoint(m_nX, m_nY, m_nZ, &nX3d, &nY3d, &nZ3d);
			ConvertTo2d(nX3d, nY3d, nZ3d, &nCurrentX, &nCurrentY);

			DrawLine(m_pLastX[0],
				-m_pLastY[0],
				nCurrentX,
				-nCurrentY,
				lpGraph);

			/*
			 * X axis loop
			 */
			 /* huh?  I thought I had to start at 1 -- */
			for (dwX = 0; dwX < m_dwResolution; dwX++)
			{
				m_nX = dwX * nResolutionInverse - 1.0f;  /* set x */;
				m_nY = m_pfValues.EvaluateFunction(dwFunction, 0);
				//                m_nZ = m_nZ;
				m_nStep = (value_t)sqrt(m_nX * m_nX + m_nZ * m_nZ);



				nLastX = nCurrentX;
				nLastY = nCurrentY;

				m_pr3dRotator.RotatePoint(m_nX, m_nY, m_nZ,
					&nX3d, &nY3d, &nZ3d);
				ConvertTo2d(nX3d, nY3d, nZ3d, &nCurrentX, &nCurrentY);

				DrawLine(m_pLastX[dwX], -m_pLastY[dwX],
					nCurrentX, -nCurrentY, lpGraph);
				DrawLine(nLastX, -nLastY, nCurrentX, -nCurrentY, lpGraph);

				/*
				 * save the current point for when dwZ = dwZ + 1
				 */
				m_pLastX[dwX] = nCurrentX;
				m_pLastY[dwX] = nCurrentY;

			}/* X loop */

		} /* Z loop */


	} /* function loop */
}


#endif

