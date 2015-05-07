/*******************************************************************************
 *
 * Module Name: dbfileio - Debugger file I/O commands. These can't usually
 *              be used when running the debugger in Ring 0 (Kernel mode)
 *
 ******************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2015, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#include "acpi.h"
#include "accommon.h"
#include "acdebug.h"
#include "actables.h"

#if (defined ACPI_DEBUGGER || defined ACPI_DISASSEMBLER)

#define _COMPONENT          ACPI_CA_DEBUGGER
        ACPI_MODULE_NAME    ("dbfileio")

#ifdef ACPI_DEBUGGER

/*******************************************************************************
 *
 * FUNCTION:    AcpiDbCloseDebugFile
 *
 * PARAMETERS:  None
 *
 * RETURN:      None
 *
 * DESCRIPTION: If open, close the current debug output file
 *
 ******************************************************************************/

void
AcpiDbCloseDebugFile (
    void)
{

#ifdef ACPI_APPLICATION

    if (AcpiGbl_DebugFile)
    {
       fclose (AcpiGbl_DebugFile);
       AcpiGbl_DebugFile = NULL;
       AcpiGbl_DbOutputToFile = FALSE;
       AcpiOsPrintf ("Debug output file %s closed\n", AcpiGbl_DbDebugFilename);
    }
#endif
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbOpenDebugFile
 *
 * PARAMETERS:  Name                - Filename to open
 *
 * RETURN:      None
 *
 * DESCRIPTION: Open a file where debug output will be directed.
 *
 ******************************************************************************/

void
AcpiDbOpenDebugFile (
    char                    *Name)
{

#ifdef ACPI_APPLICATION

    AcpiDbCloseDebugFile ();
    AcpiGbl_DebugFile = fopen (Name, "w+");
    if (!AcpiGbl_DebugFile)
    {
        AcpiOsPrintf ("Could not open debug file %s\n", Name);
        return;
    }

    AcpiOsPrintf ("Debug output file %s opened\n", Name);
    ACPI_STRNCPY (AcpiGbl_DbDebugFilename, Name,
        sizeof (AcpiGbl_DbDebugFilename));
    AcpiGbl_DbOutputToFile = TRUE;

#endif
}
#endif


#ifdef ACPI_APPLICATION
#include "acapps.h"

/*******************************************************************************
 *
 * FUNCTION:    AeLocalLoadTable
 *
 * PARAMETERS:  Table           - pointer to a buffer containing the entire
 *                                table to be loaded
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to load a table from the caller's
 *              buffer. The buffer must contain an entire ACPI Table including
 *              a valid header. The header fields will be verified, and if it
 *              is determined that the table is invalid, the call will fail.
 *
 ******************************************************************************/

static ACPI_STATUS
AeLocalLoadTable (
    ACPI_TABLE_HEADER       *Table)
{
    ACPI_STATUS             Status = AE_OK;
/*    ACPI_TABLE_DESC         TableInfo; */


    ACPI_FUNCTION_TRACE (AeLocalLoadTable);
#if 0


    if (!Table)
    {
        return_ACPI_STATUS (AE_BAD_PARAMETER);
    }

    TableInfo.Pointer = Table;
    Status = AcpiTbRecognizeTable (&TableInfo, ACPI_TABLE_ALL);
    if (ACPI_FAILURE (Status))
    {
        return_ACPI_STATUS (Status);
    }

    /* Install the new table into the local data structures */

    Status = AcpiTbInitTableDescriptor (&TableInfo);
    if (ACPI_FAILURE (Status))
    {
        if (Status == AE_ALREADY_EXISTS)
        {
            /* Table already exists, no error */

            Status = AE_OK;
        }

        /* Free table allocated by AcpiTbGetTable */

        AcpiTbDeleteSingleTable (&TableInfo);
        return_ACPI_STATUS (Status);
    }

#if (!defined (ACPI_NO_METHOD_EXECUTION) && !defined (ACPI_CONSTANT_EVAL_ONLY))

    Status = AcpiNsLoadTable (TableInfo.InstalledDesc, AcpiGbl_RootNode);
    if (ACPI_FAILURE (Status))
    {
        /* Uninstall table and free the buffer */

        AcpiTbDeleteTablesByType (ACPI_TABLE_ID_DSDT);
        return_ACPI_STATUS (Status);
    }
#endif
#endif

    return_ACPI_STATUS (Status);
}
#endif


/*******************************************************************************
 *
 * FUNCTION:    AcpiDbGetTableFromFile
 *
 * PARAMETERS:  Filename        - File where table is located
 *              ReturnTable     - Where a pointer to the table is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Load an ACPI table from a file
 *
 ******************************************************************************/

ACPI_STATUS
AcpiDbGetTableFromFile (
    char                    *Filename,
    ACPI_TABLE_HEADER       **ReturnTable,
    BOOLEAN                 MustBeAmlFile)
{
#ifdef ACPI_APPLICATION
    ACPI_STATUS             Status;
    ACPI_TABLE_HEADER       *Table;
    BOOLEAN                 IsAmlTable = TRUE;


    Status = AcpiUtReadTableFromFile (Filename, &Table);
    if (ACPI_FAILURE (Status))
    {
        return (Status);
    }

    if (MustBeAmlFile)
    {
        IsAmlTable = AcpiUtIsAmlTable (Table);
        if (!IsAmlTable)
        {
            ACPI_EXCEPTION ((AE_INFO, AE_OK,
                "Input for -e is not an AML table: "
                "\"%4.4s\" (must be DSDT/SSDT)",
                Table->Signature));
            return (AE_TYPE);
        }
    }

    if (IsAmlTable)
    {
        /* Attempt to recognize and install the table */

        Status = AeLocalLoadTable (Table);
        if (ACPI_FAILURE (Status))
        {
            if (Status == AE_ALREADY_EXISTS)
            {
                AcpiOsPrintf ("Table %4.4s is already installed\n",
                    Table->Signature);
            }
            else
            {
                AcpiOsPrintf ("Could not install table, %s\n",
                    AcpiFormatException (Status));
            }

            return (Status);
        }

        AcpiTbPrintTableHeader (0, Table);

        fprintf (stderr,
            "Acpi table [%4.4s] successfully installed and loaded\n",
            Table->Signature);
    }

    AcpiGbl_AcpiHardwarePresent = FALSE;
    if (ReturnTable)
    {
        *ReturnTable = Table;
    }


#endif  /* ACPI_APPLICATION */
    return (AE_OK);
}

#endif  /* ACPI_DEBUGGER */
